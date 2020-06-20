#pragma once

#include <array>
#include <optional>
#include <cstdint>
#include <vector>
#include <map>

#include "cartridge.hpp"
#include "debug_types.hpp"

namespace gameboy
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

    explicit Memory(Processor &c, Ppu &p, Timer &t, Joypad &j, Apu &a);

    uint8_t read(uint16_t adr) const;
    void write(uint8_t b, uint16_t adr);
    // vram read/write so PPU can read from both VRAM banks at the same time
    uint8_t vram_read(uint8_t bank, uint16_t adr) const;
    void vram_write(uint8_t b, uint8_t bank, uint16_t adr);
    Cartridge *load_cartridge(std::istream &is);
    void enable_cgb(bool is_cgb);
    void reset();

    std::vector<uint8_t> dump_rom() const;
    std::unordered_map<std::string, Memory_range> dump() const;
    void enable_logging(bool); // internally logs memory values that change
    std::vector<Memory_byte> log(); // get latest memory changes
    std::vector<uint8_t> dump_sram() const;
    bool sram_changed() const;
    bool load_save(const std::string &path);
    void hblank_dma(); // called by PPU during HBLANK

    private:
    void set_ram_size();
    void init_io();
    void oam_dma_transfer(uint8_t b);
    void vram_dma_transfer(uint8_t b); // CGB only
    void general_dma();
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


    // for debugger purposes
    bool logging_ {true};
    std::vector<Memory_byte> log_; // changed memory values

};

}
