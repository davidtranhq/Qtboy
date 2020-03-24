#include "system.hpp"
#include "debugger.hpp"

#include <iostream>
#include <string>
#include <chrono>

using namespace gameboy;

int main(int argc, char *argv[])
{
    std::string rom_path;
    System s;
    Debugger d {&s}; // attach system to debugger
    // enable logging flag
    if (argc > 1 && (std::string {argv[1]} == "-l"))
    {
        std::cout << "Logging is enabled.\n";
        d.enable_logging(true);
        if (!d.set_log_file("C:/Users/david/Documents/Github/Gameboy/tools/cpu.log"))
        {
            std::cerr << "Could not open log file.\n";
            return -1;
        }
    }
    bool valid_rom {false};
    std::cout << "Gameboy v0.1\n";
    for (;;)
    {
        std::cout << "Enter a path for of a ROM to load: ";
        std::getline(std::cin, rom_path);
        valid_rom = s.load_cartridge(rom_path);
        if (valid_rom)
            break;
        std::cout << "Could not find " << rom_path << '\n';
    }
    std::cout << "Loaded " << rom_path << ".\n";
    std::cout << "Number of instructions to log: ";
    size_t steps {};
    if (std::cin >> steps)
	{
		size_t i;
		auto start = std::chrono::high_resolution_clock::now();
		for (i = 0; i < steps; ++i)
			d.step();
		auto finish = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish-start);
		std::cout << "Took " << i << " CPU steps. (" << s.cycles() << " cycles )\n";
				  << "Took " << duration.count() << " microseconds ( " s.cycles/
				  << '(' << duration.count()/1000000 << " s).\n";
	}
	else
	{
		for (;;)
			d.step();
	}
    return 0;
}
