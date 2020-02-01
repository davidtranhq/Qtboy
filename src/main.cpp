#include "system.hpp"
#include "debugger.hpp"

#include <iostream>
#include <string>
#include <chrono>

using namespace gameboy;

int main()
{
    std::string rom_path;
    System s;
    Debugger d {&s}; // attach system to debugger
    d.enable_logging(true);

    bool valid_rom {false};
    std::cout << "Gameboy v0.1\n";
    for (;;)
    {
        std::cout << "Enter a path for of a ROM to load: ";
        std::cin >> rom_path;
        valid_rom = s.load_cartridge(rom_path);
        if (valid_rom)
            break;
        std::cout << "Could not find " << rom_path << '\n';
    }
    std::cout << "Loaded " << rom_path << ".\n";
    std::cout << "Number of instructions to log: ";
    size_t steps {};
    std::cin >> steps;
	size_t i;
    auto start = std::chrono::high_resolution_clock::now();
    for (i = 0; i < steps; ++i)
        d.step();
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish-start);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    std::cout << "Logged " << i << " CPU steps.\n"
              << "Took " << duration.count() << " microseconds "
              << '(' << ms.count() << " ms).\n";
    return 0;
}
