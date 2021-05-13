#pragma once

#include <array>
#include <optional>
#include <cstdint>
#include <vector>
#include <map>

#include "cartridge.hpp"
#include "debug_types.hpp"

namespace qtboy
{

class Timer;
class Ppu;
class Joypad;
class Apu;
class Processor;


class Memory
{
    static constexpr auto RAM_BANK_SIZE = 0x2000; // 8 KB
    public:

    // Type containing everything needed for restoring a save state
    // (VRAM, WRAM, OAM, IO, HRAM, IME). Does not include ROM.
    struct Dump;

    // References to other components are needed to access their internal registers.
    explicit Memory(Processor &c, Ppu &p, Timer &t, Joypad &j, Apu &a);

    // Read a byte from a specified address.
    uint8_t read(uint16_t adr) const;

    // Write a byte to a specified address.
    void write(uint8_t b, uint16_t adr);

    // The PPU needs the following VRAM read/write methods
    // to be able to pull data from multiple VRAM banks at once.

    // Read from a specified VRAM bank (including ones not currently mapped in memory).
    uint8_t vram_read(uint8_t bank, uint16_t adr) const;

    // Write to a specified VRAM bank (including ones not currently mapped in memory).
    void vram_write(uint8_t b, uint8_t bank, uint16_t adr);

    // Create and load a cartridge from a specified input stream pointing to a valid ROM file.
    Cartridge *load_cartridge(std::istream &is);

    // Enable/disable CGB mode. This is done automatically when loading a CGB cartridge
    // but can also be disabled manually to force DMG mode.
    void enable_cgb(bool is_cgb);

    // Reset all components to their initial states.
    void reset();

    // Dump all memory components (even the ones not mapped in memory).
    Dump dump_memory() const;

    // Copy a memory dump into memory.
    void load_memory(const Dump &dump);

    // Dump the currently mapped regions of memory.
    std::unordered_map<std::string, Memory_range> dump_mapped() const;

    // Dump the entire ROM (even banks not mapped in memory).
    std::vector<uint8_t> dump_rom() const;

    // Dump the RAM on cartridge (for generating save files).
    std::vector<uint8_t> dump_sram() const;

    // Load a save file into SRAM.
    bool load_save(const std::string &path);

    // Check if any data has been written to SRAM.
    bool sram_changed() const;

    void hblank_dma(); // called by PPU during HBLANK

    // Enable or disable debug mode (enables or disables the callback on memory write).
    void set_debug_mode(bool);

    // Set a callback to be called after each memory write while in debug mode. The callback should
    // return void and take arguments of type uint8_t and uint16_t, representing the byte written
    // and the address of the write, respectively.
    void set_debug_callback(std::function<void(uint8_t, uint16_t)>);

    void enable_logging(bool); // internally logs memory values that change
    std::vector<Memory_byte> log(); // get latest memory changes

    private:
    void set_ram_size();
    void init_io();
    void oam_dma_transfer(uint8_t b);
    void vram_dma_transfer(uint8_t b); // CGB only
    void general_dma(uint8_t hdma_len);
    void dma_copy(); // used by HDMA and GDMA
    void update_log(uint8_t b, uint16_t adr);

    private:
    std::unique_ptr<Cartridge> cart_ {nullptr};
    Video_ram vram_ {2}; // 2 banks of 8KB VRAM in CGB
    Work_ram wram_ {8}; // 8 banks of 4KB RAM in CGB
    std::array<uint8_t, 0xa0> oam_ {};
    std::array<uint8_t, 0x80> io_ {};
    std::array<uint8_t, 0x7f> hram_ {};
    Processor &cpu_; // to access CGB double speed
    Ppu &ppu_; // to access hardware registers
    Timer &timer_; // to access hardware registers
    Joypad &joypad_; // to access hardware registers
    Apu &apu_; // access hardware registers
    uint8_t ie_ {};
    bool cgb_mode_ {false};
    bool hdma_active_ {false};
    uint16_t hdma_src_ {0};
    uint16_t hdma_dest_ {0};
    uint8_t hdma_len_ {0xff};
    mutable bool sram_written_ {false};


    // Flag indciating whether debug mode is active.
    bool debug_mode_ {false};

    // Optional callback to be called after each memory write while in debug mode. Takes the byte
    // written and the address as arguments.
    std::function<void(uint8_t, uint16_t)> debug_callback_;
    bool logging_ {true};
    std::vector<Memory_byte> log_; // changed memory values

};

struct Memory::Dump
{
    Video_ram vram;
    Work_ram wram;
    std::array<uint8_t, 0xa0> oam {};
    std::array<uint8_t, 0x80> io {};
    std::array<uint8_t, 0x7f> hram {};
    uint8_t ie {};
};

}
