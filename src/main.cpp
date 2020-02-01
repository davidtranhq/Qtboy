#include "system.hpp"
#include "debugger.hpp"

#include <iostream>
#include <string>

using namespace gameboy;

int main()
{
    const std::string rom_path
        {"C:/Users/david/Documents/GitHub/Gameboy/roms/01-special.gb"};
    const std::string log_path
        {"C:/Users/david/Documents/Github/Gameboy/tools/cpu.log"};
    System s;
    s.load_cartridge(rom_path);
    Debugger d {&s};
    d.enable_logging(true);
    d.set_log_file(log_path);
    std::cout << "Number of instructions to log: ";
    size_t steps {};
    std::cin >> steps;
    while (steps-- > 0)
        d.step();
    std::cout << "Finish.\n";
    return 0;
}
