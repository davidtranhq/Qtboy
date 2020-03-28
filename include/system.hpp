#pragma once

#include <cstdint>
#include <istream>
#include <map>

#include "processor.hpp"
#include "memory.hpp"
#include "disassembler.hpp"
#include "ppu.hpp"
#include "renderer.hpp"
#include "debug_types.hpp"
#include "timer.hpp"
#include "joypad.h"

namespace gameboy
{

class System
{
    public:

    explicit System(Renderer *r = nullptr);

    // system control
    void run [[ noreturn ]] ();
    void pause();
    void run_concurrently();
    void reset();
    size_t step(size_t n); // take n CPU steps
    size_t execute(size_t cyc); // run for cyc cycles
    void press(Joypad::Input);
    void release(Joypad::Input);
    size_t cycles() { return cpu_.cycles(); }

    // system setup
    void load_cartridge(std::istream &is);
    bool load_cartridge(const std::string &path);
    void set_renderer(Renderer *r);

    // system debug
    friend class Debugger;

	private:
    // callbacks to interface other components with memory
	uint8_t memory_read(uint16_t adr);
	void memory_write(uint8_t b, uint16_t adr);

    private:
	Processor cpu_ 
	{
		[this](uint16_t adr){ return this->memory_read(adr); }, 
		[this](uint8_t b, uint16_t adr){ this->memory_write(b, adr); }
    };
    Ppu ppu_
    {
        [this](uint16_t adr){ return this->memory_read(adr); },
        [this](uint8_t b, uint16_t adr){ this->memory_write(b, adr); },
        cpu_
    };
    Timer timer_
    {
        cpu_
    };
    Joypad joypad_
    {
        cpu_
    };
    Memory memory_
    {
        ppu_, timer_, joypad_
    };
    // the renderer is an interface to facilitate the implementation of other
    // libraries for different platforms
    std::unique_ptr<Renderer> renderer_;
    static constexpr double NANOSECONDS_PER_CYCLE {1000000000/4194304};
};
	
}
