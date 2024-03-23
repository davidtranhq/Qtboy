#include <istream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <map>
#include <thread>
#include <chrono>
#include <memory>

#include "system.hpp"
#include "exception.hpp"
#include "disassembler.hpp"

#include <SDL2/SDL.h>

using std::chrono::duration_cast;
using std::chrono::nanoseconds;

namespace qtboy
{

Gameboy::Gameboy()
{}

Gameboy::~Gameboy()
{
    stop();
    // save data on close
    std::vector<uint8_t> sram(memory_.dump_sram());
    // only save data if any save data was modified
    if (!memory_.sram_changed())
        return;
    try
    {
        write_save(sram);
        std::cout << "Data was successfully saved to " << save_dir_ << ".\n";
    }
    catch (const std::exception &e)
    {
        std::cout << "Could not save data: " << e.what() << '\n';
    }
}

// returns the filename without the last extension
std::string stem(const std::string &path)
{
    // name is in between last '/' (directory) and '.' (file extension)
    auto const start = path.find_last_of('/');
    auto const end = path.find_last_of('.');
    return path.substr(start+1, end-(start+1));
}

bool Gameboy::load_cartridge(const std::string &path)
{
    std::ifstream rom {path, std::ios::binary};
    if (!rom.good())
        return false;
    rom_title_ = stem(path);
    // load the ROM into a cartridge in memory
    Cartridge *cart {memory_.load_cartridge(rom)};
    cgb_mode_ = (cart->is_cgb() && !force_dmg_);
    ppu_.enable_cgb(cgb_mode_);
    memory_.enable_cgb(cgb_mode_);
    memory_.load_save(save_dir_ + "/" + rom_title_ + ".sav");
    rom_loaded_ = true;
    return true;
}

void Gameboy::set_renderer(Renderer *r)
{
    const std::lock_guard<std::mutex> lock(mutex_);
    ppu_.set_renderer(r);
}

void Gameboy::set_speaker(std::shared_ptr<Speaker> s)
{
    const std::lock_guard<std::mutex> lock(mutex_);
    apu_.set_speaker(std::move(s));
}

void Gameboy::run()
{
    emu_paused_ = false;
    emu_stop_ = false;
    while (!emu_stop_)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        // wait until the emulator is unpaused
        pause_cv_.wait(lock, [this]{ return !emu_paused_; });
        // only run the CPU when samples are needed => wait if enough samples are already queued
        while (apu_.samples_queued() > Apu::SAMPLE_SIZE*4 && throttle_)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        // run the CPU for 1 frame (70224 cycles)
        execute(70224);
    }
    emu_paused_ = true;
}

void Gameboy::run_concurrently()
{
    if (!rom_loaded_)
    {
        throw std::runtime_error {"Error running Gameboy: No ROM is loaded"};
    }
    stop();
    auto run_fn = [this]{ this->run(); };
    emu_thread_ = std::thread(run_fn);
}

void Gameboy::stop()
{
    emu_stop_ = true;
    if (emu_thread_.joinable())
        emu_thread_.join();
}

void Gameboy::pause()
{
    emu_paused_ = true;
}

void Gameboy::resume()
{
    if (!emu_paused_)
        return;
    std::lock_guard<std::mutex> lock(mutex_);
    emu_paused_ = false;
    // notify emu_thread_ to start running again
    pause_cv_.notify_one();
}

void Gameboy::reset()
{
    stop();
    memory_.reset();
    cpu_.reset();
    ppu_.reset();
    apu_.reset();
    timer_.reset();
    joypad_.reset();
    rom_title_ = {};
    rom_loaded_ = false;
}

size_t Gameboy::step(size_t n)
{
    size_t cycles_passed = 0;
    for (size_t i = 0; i < n; ++i)
    {
        // run an optional debug callback (set with set_debug_callback())
        // after each CPU instruction
        if (debug_mode_)
        {
            // debug callback returns a boolean indicating a desired break
            debug_break_ = cpu_debug_callback_();
            if (debug_break_)
                break;
        }
        size_t old_cycles {cpu_.cycles()};
        cpu_.step();
        cycles_passed += (cpu_.cycles() - old_cycles);
        ppu_.step(cycles_passed);
        timer_.update(cycles_passed);
        apu_.tick(cycles_passed);
    }
    return cycles_passed;
}

size_t Gameboy::execute(size_t cyc)
{
    size_t cycles_passed = 0;
    // continuously step 1 CPU instruction until the specified number of cycles have
    // passed or until debug_callback_ requests a break
    while (cycles_passed < cyc && !debug_break_)
        cycles_passed += step(1);
    return cycles_passed;
}

void Gameboy::press(Joypad::Input i)
{
    joypad_.press(i);
}

void Gameboy::release(Joypad::Input i)
{
    joypad_.release(i);
}

void Gameboy::set_throttle(bool b)
{
    throttle_ = b;
    // turn off APU if in turbo mode (to prevent large excess of samples queued)
}

void Gameboy::toggle_sound(bool b)
{
    apu_.toggle_sound(b);
}

void Gameboy::set_force_dmg(bool b)
{
    force_dmg_ = b;
}

size_t Gameboy::cycles() const
{
    return cpu_.cycles();
}

bool Gameboy::is_cgb() const
{
    return cgb_mode_;
}

bool Gameboy::is_running() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return !emu_paused_;
}

// passed as callback function to CPU
uint8_t Gameboy::memory_read(uint16_t adr)
{
    return memory_.read(adr);
}

// passed as callback function to CPU
void Gameboy::memory_write(uint8_t b, uint16_t adr)
{
    memory_.write(b, adr);
}

void Gameboy::set_debug_mode(bool b)
{
    debug_mode_ = b;
    memory_.set_debug_mode(b);
}

void Gameboy::set_cpu_debug_callback(std::function<bool()> fn)
{
    cpu_debug_callback_ = std::move(fn);
}

void Gameboy::set_memory_debug_callback(std::function<void(uint8_t, uint16_t)> fn)
{
    memory_.set_debug_callback(std::move(fn));
}

std::unordered_map<std::string, Memory_range> Gameboy::dump_mapped_memory() const
{
    return memory_.dump_mapped();
}

std::vector<Memory_byte> Gameboy::dump_memory_log()
{
    const std::lock_guard<std::mutex> lock(mutex_);
    return memory_.log();
}

std::vector<uint8_t> Gameboy::dump_rom() const
{
    return memory_.dump_rom();
}

Cpu_dump Gameboy::dump_cpu() const
{
    const std::lock_guard<std::mutex> lock(mutex_);
    return cpu_.dump();
}

Ppu::Dump Gameboy::dump_ppu() const
{
    const std::lock_guard<std::mutex> lock(mutex_);
    return ppu_.dump_values();
}

std::array<Palette, 8> Gameboy::dump_bg_palettes() const
{
    std::array<Palette, 8> pals {};
    for (uint8_t i = 0; i < 8; ++i)
        pals[i] = ppu_.get_bg_palette(i);
    return pals;
}

std::array<Palette, 8> Gameboy::dump_sprite_palettes() const
{
    std::array<Palette, 8> pals {};
    for (uint8_t i = 0; i < 8; ++i)
        pals[i] = ppu_.get_sprite_palette(i);
    return pals;
}

std::array<Texture, 384> Gameboy::dump_tileset(uint8_t bank) const
{
    std::array<Texture, 384> set {};
    for (uint16_t i = 0; i < 384; ++i)
        set[i] = ppu_.get_tile(bank, i);
    return set;
}

Texture Gameboy::dump_framebuffer(bool with_bg, bool with_win,
                                 bool with_sprites) const
{
    return ppu_.get_framebuffer(with_bg, with_win, with_sprites);
}

Texture Gameboy::dump_background() const
{
    return ppu_.get_layer(Ppu::Layer::Background);
}

Texture Gameboy::dump_window() const
{
    return ppu_.get_layer(Ppu::Layer::Window);
}

std::array<Texture, 40> Gameboy::dump_sprites() const
{
    return ppu_.get_sprites();
}


void Gameboy::write_save(const std::vector<uint8_t> &sram)
{
    const std::string path(save_dir_ + '/' + rom_title_ + ".sav");
    std::ofstream out(path, std::ios::binary);
    if (!out)
        throw std::runtime_error {"failed to open " + path};
    out.write(reinterpret_cast<const char *>(sram.data()), sram.size());
    if (!out)
        throw std::runtime_error {"error occured when writing to " + path};
}

} // namespace qtboy
