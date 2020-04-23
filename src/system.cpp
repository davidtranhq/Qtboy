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
{}

// system control

void System::run()
{
    // prevent when another is running
    const std::lock_guard<std::mutex> lock(run_mutex_);
    running_ = true;
    while (running_)
    {
        auto start = std::chrono::high_resolution_clock::now();
        size_t cycles = execute(70224); // number of cycles in one frame
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

void System::run_concurrently(std::thread &t2)
{
    auto run_fn = [this]{ this->run(); };
    if (running_)
        running_ = false;
    std::thread t1(run_fn);
    t2 = std::move(t1);
}

void System::reset()
{
    memory_.reset();
    cpu_.reset();
    ppu_.reset();
    apu_.reset();
}

size_t System::step(size_t n)
{
    size_t cycles_passed = 0;
    for (size_t i {0}; i < n; ++i)
    {
        size_t old_cycles {cpu_.cycles()};
        cpu_.step();
        cycles_passed += (cpu_.cycles() - old_cycles);
        ppu_.step(cycles_passed);
        timer_.update(cycles_passed);
        apu_.tick(cycles_passed);
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

void System::load_cartridge(std::istream &is)
{
	memory_.load_cartridge(is);
}

bool System::load_cartridge(const std::string &path)
{
	std::ifstream rom {path, std::ios::binary};
	if (!rom.good())
        return false;
	memory_.load_cartridge(rom);
    return true;
}

void System::set_renderer(Renderer *r)
{
    ppu_.set_renderer(r);
}

void System::set_speaker(Speaker *s)
{
    apu_.set_speaker(s);
}

// system debug



// callbacks to interface other components with memory

uint8_t System::memory_read(uint16_t adr)
{
	return memory_.read(adr);
}

void System::memory_write(uint8_t b, uint16_t adr)
{
	memory_.write(b, adr);
}

}
