#include <istream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <map>

#include "system.hpp"
#include "exception.hpp"
#include "disassembler.hpp"

namespace gameboy
{

System::System(Renderer *r)
    : renderer_ {r}
{}

// system control

void System::run()
{}

void System::reset()
{
    memory_.reset();
    cpu_.reset();
    ppu_.reset();
}

void System::step(size_t n)
{
    for (size_t i {0}; i < n; ++i)
    {
        size_t old_cycles {cpu_.cycles()};
        cpu_.step();
        ppu_.step(cpu_.cycles() - old_cycles);
    }
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
    renderer_.reset(r);
    ppu_.set_renderer(r);
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
