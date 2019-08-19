#pragma once

#include <cstdint>
#include <istream>

#include "processor.hpp"

namespace gameboy
{
	
class System
{
	public:
	explicit System();
	
	void run();
	void load_cartridge(std::istream &is);
	void load_cartridge(const std::string &path);
	
	private:
	Memory memory_;
	Processor cpu_ {memory_};
};
	
}