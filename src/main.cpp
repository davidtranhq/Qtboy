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
        if (!d.set_log_file("cpu.log"))
        {
            std::cerr << "Could not open log file.\n";
            return -1;
        }
    }
    bool valid_rom {false};
    std::cout << "Gameboy v0.2\n";
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
    std::cout << "Running " << rom_path << ".\n";
    d.run();
    return 0;
}
