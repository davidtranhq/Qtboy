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

class Memory
{
    static constexpr auto RAM_BANK_SIZE = 0x2000; // 8 KB
    public:

    explicit Memory(Ppu &p, Timer &t, Joypad &j);

    uint8_t read(uint16_t adr) const;
    void write(uint8_t b, uint16_t adr);
    void load_cartridge(std::istream &is);
    void reset();

    std::vector<uint8_t> dump_rom() const;
    std::unordered_map<std::string, Memory_range> dump() const;
    bool was_written();

    private:
    void set_ram_size();
    void init_io();
    void dma_transfer(uint8_t b);

    private:
    std::unique_ptr<Cartridge> cart_ {nullptr};
    std::array<uint8_t, 0x2000> vram_;
    std::array<uint8_t, 0x1000> wram0_, wram1_;
    std::array<uint8_t, 0xa0> oam_ {};
    std::array<uint8_t, 0x80> io_ {};
    std::array<uint8_t, 0x7f> hram_ {};
    Ppu &ppu_; // to access hardware registers
    Timer &timer_; // to access hardware registers
    Joypad &joypad_; // to access hardware registers
    uint8_t ie_ {};
    bool was_written_ {false};

};

}
