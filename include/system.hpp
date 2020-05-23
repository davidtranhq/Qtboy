#pragma once

#include <cstdint>
#include <istream>
#include <map>
#include <thread>
#include <mutex>
#include <atomic>

#include "processor.hpp"
#include "memory.hpp"
#include "disassembler.hpp"
#include "ppu.hpp"
#include "renderer.hpp"
#include "debug_types.hpp"
#include "timer.hpp"
#include "joypad.hpp"
#include "apu.hpp"
#include "speaker.hpp"
#include "debugger.hpp"

namespace gameboy
{

class System
{
    public:

    explicit System();

    // system control
    void run_concurrently();
    void pause();
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
    void set_speaker(Speaker *s);

    // system debug
    std::unordered_map<std::string, Memory_range> dump_memory() const;
    std::vector<Memory_byte> dump_memory_log();
    std::vector<uint8_t> dump_rom() const;
    Cpu_values dump_cpu() const;
    std::array<Texture, 384> dump_tileset() const;
    Texture dump_background() const;
    Texture dump_window() const;
    std::array<Sprite, 40> dump_sprites() const;
    void set_step_callback(std::function<void()>); // callback before every step
                                                   // useful for logging
    uint8_t memory_read(uint16_t adr);
    void memory_write(uint8_t b, uint16_t adr);
    static constexpr double NANOSECONDS_PER_CYCLE {1000000000/4194304};

    private:
    void run();

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
    Timer timer_ {cpu_};
    Joypad joypad_ {cpu_};
    Apu apu_ {};
    Memory memory_{ ppu_, timer_, joypad_, apu_};
    std::function<void()> step_callback_ {};
    std::atomic<bool> running_ {false};
    std::thread thread_ {};
    mutable std::mutex mutex_;
};


}
