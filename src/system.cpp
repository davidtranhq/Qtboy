#include <istream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <map>
#include <thread>
#include <chrono>
#include <experimental/filesystem>

#include "system.hpp"
#include "exception.hpp"
#include "disassembler.hpp"

using std::chrono::duration_cast;
using std::chrono::nanoseconds;
namespace fs = std::experimental::filesystem;

namespace gameboy
{

System::System()
{}

System::~System()
{
    std::vector<uint8_t> sram(memory_.dump_sram());
    if (!sram.empty())
        write_save(sram);
}

// system control

void System::run()
{
    running_ = true;
    while (running_)
    {
        auto start = std::chrono::high_resolution_clock::now();
        // run for one frame
        size_t cycles = execute(70224); // number of cycles in one frame
        // save SRAM to seperate file if changes were made
        auto finish = std::chrono::high_resolution_clock::now();
        double expected = NANOSECONDS_PER_CYCLE * cycles;
        auto duration = duration_cast<nanoseconds>(finish-start).count();
        if (duration < expected)
        {
            auto t = std::chrono::duration<double, std::nano>(expected-duration);
            std::this_thread::sleep_for(t);
        }
    }
}

void System::run_concurrently()
{
    auto run_fn = [this]{ this->run(); };
    thread_ = std::thread(run_fn);
}

void System::pause()
{
    running_ = false;
    if (thread_.joinable())
        thread_.join();

}

void System::reset()
{
    running_ = false;
    if (thread_.joinable())
        thread_.join();
    memory_.reset();
    cpu_.reset();
    ppu_.reset();
    apu_.reset();
    timer_.reset();
    joypad_.reset();
    step_callback_ = {};
    rom_title_ = {};
}

size_t System::step(size_t n)
{
    size_t cycles_passed = 0;
    for (size_t i {0}; i < n; ++i)
    {
        if (step_callback_)
            step_callback_();
        {
            const std::lock_guard<std::mutex> lock(mutex_);
            size_t old_cycles {cpu_.cycles()};
            cpu_.step();
            cycles_passed += (cpu_.cycles() - old_cycles);
            ppu_.step(cycles_passed);
            timer_.update(cycles_passed);
            apu_.tick(cycles_passed);
        }
    }
    return cycles_passed;
}

size_t System::execute(size_t cyc)
{
    size_t cycles_passed = 0;
    while (cycles_passed < cyc)
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

// system setup

bool System::load_cartridge(const std::string &path)
{
	std::ifstream rom {path, std::ios::binary};
	if (!rom.good())
        return false;
    rom_title_ = fs::path(path).stem().string();
    memory_.load_cartridge(rom);
    bool res = memory_.load_save(save_dir_ + "/" + rom_title_ + ".sav");
    if (res)
        std::cout << "Loaded save file.";
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

void System::set_step_callback(std::function<void()> f)
{
    const std::lock_guard<std::mutex> lock(mutex_);
    step_callback_ = std::move(f);
}

std::unordered_map<std::string, Memory_range> System::dump_memory() const
{
    const std::lock_guard<std::mutex> lock(mutex_);
    return memory_.dump();
}

std::vector<Memory_byte> System::dump_memory_log()
{
    const std::lock_guard<std::mutex> lock(mutex_);
    return memory_.log();
}

std::vector<uint8_t> System::dump_rom() const
{
    const std::lock_guard<std::mutex> lock(mutex_);
    return memory_.dump_rom();
}

Cpu_values System::dump_cpu() const
{
    const std::lock_guard<std::mutex> lock(mutex_);
    return cpu_.dump();
}

std::array<Texture, 384> System::dump_tileset() const
{
    std::array<Texture, 384> set {};
    const std::lock_guard<std::mutex> lock(mutex_);
    for (uint16_t i = 0; i < 384; ++i)
        set[i] = ppu_.get_tile(i);
    return set;
}

Texture System::dump_background() const
{
    const std::lock_guard<std::mutex> lock(mutex_);
    return ppu_.get_layer(Ppu::Layer::Background);
}

Texture System::dump_window() const
{
    const std::lock_guard<std::mutex> lock(mutex_);
    return ppu_.get_layer(Ppu::Layer::Window);
}

std::array<Texture, 40> System::dump_sprites() const
{
    const std::lock_guard<std::mutex> lock(mutex_);
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
    const std::string path(save_dir_ + '/' + rom_title_ + ".sav");
    std::ofstream f(path, std::ios::binary);
    f.write(reinterpret_cast<const char *>(sram.data()), sram.size());
}

}
