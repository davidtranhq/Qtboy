#include <istream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <map>
#include <thread>
#include <chrono>

#include "system.hpp"
#include "exception.hpp"
#include "disassembler.hpp"

using std::chrono::duration_cast;
using std::chrono::nanoseconds;

namespace gameboy
{

System::System()
{
}

System::~System()
{
    std::vector<uint8_t> sram(memory_.dump_sram());
    if (!sram.empty())
        write_save(sram);
}

// system control

void System::run()
{
    while (emu_running_)
    {
        auto start = std::chrono::high_resolution_clock::now();
        // run for one frame
        size_t cycles = execute(70224); // number of cycles in one frame
        auto finish = std::chrono::high_resolution_clock::now();
        double expected = NANOSECONDS_PER_CYCLE * cycles;
        auto duration = duration_cast<nanoseconds>(finish-start).count();
        // don't framelimit if throttle is 0
        if (throttle_ <= 0.0)
            continue;
        else
            expected /= throttle_;
        if (duration < expected)
        {
            auto t = std::chrono::duration<double, std::nano>(expected-duration);
            std::this_thread::sleep_for(t);
        }
    }
}

void System::run_concurrently()
{
    emu_running_ = true;
    emu_thread_.queue();
}

void System::pause()
{
    emu_running_ = false;
}

void System::reset()
{
    emu_running_ = false;
    memory_.reset();
    cpu_.reset();
    ppu_.reset();
    apu_.reset();
    timer_.reset();
    joypad_.reset();
    rom_title_ = {};
}

size_t System::step(size_t n)
{
    size_t cycles_passed = 0;
    for (size_t i {0}; i < n; ++i)
    {
        emu_running_ = true;
        if (debugging_)
            debug_callback_();
        // check if emu was paused because of debugger
        if (emu_running_)
        {
            const std::lock_guard<std::mutex> lock(mutex_);
            size_t old_cycles {cpu_.cycles()};
            cpu_.step();
            cycles_passed += (cpu_.cycles() - old_cycles);
            ppu_.step(cycles_passed);
            timer_.update(cycles_passed);
            apu_.tick(cycles_passed);
        }
        else
        {
            break;
        }
    }
    return cycles_passed;
}

size_t System::execute(size_t cyc)
{
    size_t cycles_passed = 0;
    while (cycles_passed < cyc && emu_running_)
        cycles_passed += step(1);
    return cycles_passed;
}

void System::press(Joypad::Input i)
{
    joypad_.press(i);
}

void System::release(Joypad::Input i)
{
    joypad_.release(i);
}

void System::set_throttle(double d)
{
    throttle_ = d;
}

void System::set_debug(bool b)
{
    debugging_ = b;
}

bool System::is_running()
{
    return emu_running_;
}

// system setup

// returns the filename without the last extension
static std::string stem(const std::string &path)
{
    // name is in between last '/' (directory) and '.' (file extension)
    auto const start = path.find_last_of('/');
    auto const end = path.find_last_of('.');
    return path.substr(start+1, end-(start+1));
}

bool System::load_cartridge(const std::string &path)
{
	std::ifstream rom {path, std::ios::binary};
	if (!rom.good())
        return false;
    rom_title_ = stem(path);
    cgb_mode_ = (memory_.load_cartridge(rom)->is_cgb() && !force_dmg);
    ppu_.enable_cgb(cgb_mode_);
    memory_.enable_cgb(cgb_mode_);
    memory_.load_save(save_dir + "/" + rom_title_ + ".sav");
    return true;
}

void System::set_renderer(Renderer *r)
{
    const std::lock_guard<std::mutex> lock(mutex_);
    ppu_.set_renderer(r);
}

void System::set_speaker(Speaker *s)
{
    const std::lock_guard<std::mutex> lock(mutex_);
    apu_.set_speaker(s);
}

// system debug

void System::set_debug_callback(std::function<void()> fn)
{
    debug_callback_ = std::move(fn);
}

std::unordered_map<std::string, Memory_range> System::dump_memory() const
{
    return memory_.dump();
}

std::vector<Memory_byte> System::dump_memory_log()
{
    const std::lock_guard<std::mutex> lock(mutex_);
    return memory_.log();
}

std::vector<uint8_t> System::dump_rom() const
{
    return memory_.dump_rom();
}

Cpu_dump System::dump_cpu() const
{
    const std::lock_guard<std::mutex> lock(mutex_);
    return cpu_.dump();
}

Ppu::Dump System::dump_ppu() const
{
    const std::lock_guard<std::mutex> lock(mutex_);
    return ppu_.dump_values();
}

std::array<Palette, 8> System::dump_bg_palettes() const
{
    std::array<Palette, 8> pals {};
    for (uint8_t i = 0; i < 8; ++i)
        pals[i] = ppu_.get_bg_palette(i);
    return pals;
}

std::array<Palette, 8> System::dump_sprite_palettes() const
{
    std::array<Palette, 8> pals {};
    for (uint8_t i = 0; i < 8; ++i)
        pals[i] = ppu_.get_sprite_palette(i);
    return pals;
}

std::array<Texture, 384> System::dump_tileset(uint8_t bank) const
{
    std::array<Texture, 384> set {};
    for (uint16_t i = 0; i < 384; ++i)
        set[i] = ppu_.get_tile(bank, i);
    return set;
}

Texture System::dump_framebuffer(bool with_bg, bool with_win,
                                 bool with_sprites) const
{
    return ppu_.get_framebuffer(with_bg, with_win, with_sprites);
}

Texture System::dump_background() const
{
    return ppu_.get_layer(Ppu::Layer::Background);
}

Texture System::dump_window() const
{
    return ppu_.get_layer(Ppu::Layer::Window);
}

std::array<Texture, 40> System::dump_sprites() const
{
    return ppu_.get_sprites();
}

// callbacks to interface other components with memory

uint8_t System::memory_read(uint16_t adr)
{
	return memory_.read(adr);
}

void System::memory_write(uint8_t b, uint16_t adr)
{
	memory_.write(b, adr);
}



void System::write_save(const std::vector<uint8_t> &sram)
{
    const std::string path(save_dir + '/' + rom_title_ + ".sav");
    std::ofstream f(path, std::ios::binary);
    f.write(reinterpret_cast<const char *>(sram.data()), sram.size());
}

} // namespace gameboy
