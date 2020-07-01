#pragma once

#include <cstdint>
#include <istream>
#include <map>
#include <optional>
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
#include "reusable_thread.hpp"

namespace gameboy
{

class System
{
    public:

    explicit System();
    ~System();

    // system control
    void run_concurrently();
    void pause();
    void reset();
    size_t step(size_t n); // take n CPU steps
    size_t execute(size_t cyc); // run for cyc cycles
    void press(Joypad::Input);
    void release(Joypad::Input);
    size_t cycles() const { return cpu_.cycles(); }
    bool is_cgb() const { return cgb_mode_; }
    bool is_running();
    void set_throttle(double factor);
    void set_debug(bool b);

    // system setup
    bool load_cartridge(const std::string &path);
    void set_renderer(Renderer *r);
    void set_speaker(Speaker *s);

    // system debug
    void set_debug_callback(std::function<void()>); // callback before every step
                                                   // useful for logging
    std::unordered_map<std::string, Memory_range> dump_memory() const;
    std::vector<Memory_byte> dump_memory_log();
    std::vector<uint8_t> dump_rom() const;
    Cpu_dump dump_cpu() const;
    Ppu::Dump dump_ppu() const;
    std::array<Palette, 8> dump_bg_palettes() const;
    std::array<Palette, 8> dump_sprite_palettes() const;
    std::array<Texture, 384> dump_tileset(uint8_t bank) const;
    Texture dump_framebuffer(bool with_bg = true, bool with_win = true,
                             bool with_sprites = true) const;
    Texture dump_background() const;
    Texture dump_window() const;
    std::array<Texture, 40> dump_sprites() const;
    uint8_t memory_read(uint16_t adr);
    void memory_write(uint8_t b, uint16_t adr);
    static constexpr double NANOSECONDS_PER_CYCLE {1000000000/4194304};

    public:
    // configurable options
    std::string save_dir {"saves"};
    bool force_dmg {false};

    private:
    void update_debugger();
    void write_save(const std::vector<uint8_t> &sram);
    void run();

    private:
    Reusable_thread emu_thread_ { [this]{run();} };
    std::function<void()> debug_callback_;
    std::atomic<bool> emu_running_ {false};
    mutable std::mutex mutex_;
	Processor cpu_ 
	{
		[this](uint16_t adr){ return this->memory_read(adr); }, 
		[this](uint8_t b, uint16_t adr){ this->memory_write(b, adr); }
    };
    Ppu ppu_
    {
        memory_,
        cpu_
    };
    Timer timer_ {cpu_};
    Joypad joypad_ {cpu_};
    Apu apu_ {};
    Memory memory_ { cpu_, ppu_, timer_, joypad_, apu_};
    std::string rom_title_ {};
    bool cgb_mode_ {!force_dmg};
    std::atomic<double> throttle_ {1.0};
    bool debugging_ {false};
};


}
