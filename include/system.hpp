#pragma once

#include <istream>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>

#include "processor.hpp"
#include "memory.hpp"
#include "ppu.hpp"
#include "renderer.hpp"
#include "debug_types.hpp"
#include "timer.hpp"
#include "joypad.hpp"
#include "apu.hpp"
#include "speaker.hpp"
#include "debugger.hpp"

namespace qtboy
{

class Gameboy
{
    public:

    explicit Gameboy();
    ~Gameboy();

    // Start the emulator in a seperate thread. load_cartridge() must be called before this to ensure
    // a ROM is loaded. If the emulator is already running in a thread, the previous thread is
    // killed and the emulator restarted (not reset) in a new thread.
    void run_concurrently();

    // Load the ROM file at path into memory.
    bool load_cartridge(const std::string &path);

    // Set the renderer to use to display video output. This must be set for video output.
    void set_renderer(Renderer *r);

    // Set the speaker to use to play audio output. This must be set for audio output.
    void set_speaker(std::shared_ptr<Speaker> s);

    // Stop the emulator that is currently running from a call to run_concurrently().
    void stop();

    // Pause the emulator. The emulation thread is kept alive and can be resumed with resume().
    void pause();

    // Resumes the emulator paused with pause().
    void resume();

    // Stops the emulator, unloads the ROM, and resets the memory, CPU, PPU, APU, timer, and joypad.
    void reset();

    // Step through n CPU instructions.
    size_t step(size_t n);

    // Run the CPU for cyc cycles.
    size_t execute(size_t cyc);

    // Press a specified input on the joypad.
    void press(Joypad::Input i);

    // Release a specified input on the joypad.
    void release(Joypad::Input i);

    // Enables or disables CPU throttling (unlimited FPS)
    void set_throttle(bool b);

    void toggle_sound(bool b);

    void set_force_dmg(bool b);

    // Get the total number of cycles ran by the CPU.
    size_t cycles() const;

    // Returns true if the emulator is running in CGB mode.
    bool is_cgb() const;

    // Returns true if the emulator is not paused nor stopped.
    bool is_running() const;

    // memory read/write functions passed as callbacks to CPU
    uint8_t memory_read(uint16_t adr);
    void memory_write(uint8_t b, uint16_t adr);

    //
    // Debugger methods
    //

    // Enables or disables the calling of debug_callback_ after every CPU instruction.
    void set_debug_mode(bool b);

    // Set a callback funciton that will be called after every CPU instruction in debug mode. The function
    // should take no arguments and return a boolean indicating if emulation should break.
    void set_cpu_debug_callback(std::function<bool()>);

    // Set a callback function that will be called after every memory write in debug mode.
    // The function should take in a uint8_t and uint16_t, representing the byte and address
    // written.
    void set_memory_debug_callback(std::function<void(uint8_t, uint16_t)>);

    // Dump the values currently mapped in memory.
    std::unordered_map<std::string, Memory_range> dump_mapped_memory() const;

    // Dump the values in memory that have been changed since the last memory log dump.
    std::vector<Memory_byte> dump_memory_log();

    // Dump the currently loaded ROM.
    std::vector<uint8_t> dump_rom() const;

    // Dump the CPU registers, number of cycles, IME, and next 3 instructions.
    Cpu_dump dump_cpu() const;

    // Dump the PPU registers.
    Ppu::Dump dump_ppu() const;

    // Dump the CGB color palettes held in memory (or the grayscale DMG palettes if not in CGB mode
    std::array<Palette, 8> dump_bg_palettes() const;
    std::array<Palette, 8> dump_sprite_palettes() const;

    // Dump the active tileset loaded in memory
    std::array<Texture, 384> dump_tileset(uint8_t bank) const;

    // Dump the full 256x256 frame buffer. with_bg, with_win, with_sprites specifies whether
    // the background, window, and sprite layers are drawn on the frame buffer, respectively.
    Texture dump_framebuffer(bool with_bg = true,
                             bool with_win = true,
                             bool with_sprites = true) const;

    // Dump the background layer.
    Texture dump_background() const;

    // Dump the window layer.
    Texture dump_window() const;

    // Dump the 40 sprites currently loaded in memory.
    std::array<Texture, 40> dump_sprites() const;

    private:
    // Write the contents of sram to a binary .sav file in save_dir_
    void write_save(const std::vector<uint8_t> &sram);

    // Runs the emulator. This is passed to emu_thread_ in run_concurrently().
    void run();

    private:
    // Title of currently loaded ROM
    std::string rom_title_ {};

    // Flag indicated if ROM is loaded (using load_cartridge())
    bool rom_loaded_ {false};

    // Flag indicating if CGB mode is enabled. CGB mode is automatically turned on if a CGB
    // ROM is loaded (and force_dmg_ is false).
    bool cgb_mode_ {!force_dmg_};

    // Directory to where .sav files are saved
    std::string save_dir_ {"saves"};

    // Option to force DMG mode on CGB cartridges
    bool force_dmg_ {false};

    // Option to enable/disable CPU throttling
    std::atomic<bool> throttle_ {true};

    // Thread for running concurrent emulation
    std::thread emu_thread_;

    // Flag to indicate whether the emulation should be stopped (can't be resumed)
    std::atomic<bool> emu_stop_ {false};

    // Flag to indicate if the emulation is paused (can be resumed)
    bool emu_paused_ {false};

    // Control multithreaded execution (between run(), pause(), resume(), and stop())
    std::condition_variable pause_cv_;
    mutable std::mutex mutex_;

    bool debug_mode_ {false};

    // Optional callback function to be executed after each CPU instruction when in debug mode
    std::function<bool()> cpu_debug_callback_;

    // Assigned by the return value of debug_callback_ to indicate if emulation should break
    bool debug_break_ {false};


	Processor cpu_ 
	{
        [this](uint16_t adr){ return this->memory_read(adr); }, // memory read callback
        [this](uint8_t b, uint16_t adr){ this->memory_write(b, adr); } // memory write callback
    };
    Ppu ppu_
    {
        // reference to memory so PPU can access different VRAM banks directly and execute DMAs
        memory_,
        // reference to CPU so PPU can request LCD, STAT, VBLANK interrupts
        cpu_
    };
    Timer timer_ {cpu_}; // reference to CPU so timer can request Timer interrupt
    Joypad joypad_ {cpu_}; // reference to CPU so joypad can request Joypad interrupt
    Apu apu_ {};
    // references to other components so that memory bus can access their internal registers
    Memory memory_ { cpu_, ppu_, timer_, joypad_, apu_};
};


}
