#include "debugger.hpp"
#include "system.hpp"

#include <sstream>
#include <iomanip>

namespace gameboy
{

Debugger::Debugger(System *s)
    : system_ {s},
      breaks_ {}
{}

void Debugger::run()
{
    for (;;)
    {
        if (at_breakpoint())
            break;
        system_->step(1);
        ++steps_;
    }
}

void Debugger::step(size_t n)
{
    for (size_t i {0}; i < n; ++i)
    {
        system_->step(1);
        ++steps_;
        if (at_breakpoint())
            break;
    }
}

void Debugger::add_breakpoint(uint16_t adr)
{
    breaks_[adr] = true;
}

void Debugger::delete_breakpoint(uint16_t adr)
{
    breaks_[adr] = false;
}

bool Debugger::at_breakpoint()
{
    uint16_t pc {dump_cpu().pc};
    if (breaks_[pc])
        return true;
    return false;
}

std::vector<uint8_t> Debugger::dump_memory() const
{
    std::map<std::string, Memory_range> mem {dump_mapped_memory()};
    std::vector<uint8_t> out {};
    for (const std::string &r : Memory_range_names)
        out.insert(out.end(), mem[r].data.begin(), mem[r].data.end());
    return out;
}

std::map<std::string, Memory_range> Debugger::dump_mapped_memory() const
{
    std::map<std::string, Memory_range> out {system_->memory_.dump()};
    std::vector<uint8_t> echo_ram {out["WRM0"].data};
    echo_ram.insert(echo_ram.end(), out["WRMX"].data.begin(), out["WRMX"].data.begin() + 0x0e00);
    out["ECHO"] = {"ECHO", echo_ram};
    out["XXXX"] = {"UNUS", std::vector<uint8_t>(0x60)};
    return out;
}

std::string Debugger::dump_formatted_memory(Dump_format d) const
{
    static std::map<std::string, Memory_range> mem {dump_mapped_memory()};
    if (memory_changed())
        mem = dump_mapped_memory();
    std::ostringstream dump {};
    std::ostringstream out {};
    out << std::hex << std::setfill('0');
    size_t i {0};
    switch (d)
    {
        case Dump_format::Hex:
        {
            for (const std::string &r : Memory_range_names)
            {
                const std::vector<uint8_t> &v {mem[r].data};
                // rows + data
                for (size_t j {0}; j < v.size(); j += 16, i += 16)
                {
                    // row marker
                    out << std::setfill(' ') << std::setw(4) << mem[r].name << ':'
                        << std::setw(4) << std::setfill('0') << std::hex
                        << i << ' ';
                    // line of 16 bytes
                    std::string line;
                    for (unsigned short k {0}; k < 16; ++k)
                    {
                        const uint8_t c {v[j+k]};
                        if (c < 32 || c > 126) // non-printable ASCII
                            line += '.';
                        else
                            line += static_cast<char>(c);
                        out << std::setw(2) << std::setfill('0') << std::hex
                            << static_cast<int>(c) << ' ';
                    }
                    out << line << '\n';
                }
            }
        } break;
        case Dump_format::Stack:
        {
            i = 0xfffe;
            for (auto n {Memory_range_names.rbegin()};
                 n != Memory_range_names.rend();
                 ++n)
            {
                const std::vector<uint8_t> &v {mem[*n].data};
                for (auto j {v.rbegin()}; j != v.rend(); ++j, --i)
                {
                    out << std::setfill(' ') << std::setw(4) << mem[*n].name << ':'
                        << std::setw(4) << std::setfill('0') << std::hex
                        << i << ' '
                        << std::setw(2) << static_cast<int>(*j)
                        << '\n';
                }
            }
        } break;
    }
    return out.str();
}

bool Debugger::memory_changed() const
{
    return system_->memory_changed_;
}

std::vector<uint8_t> Debugger::dump_rom() const noexcept
{
    return system_->memory_.dump_rom();
}

Cpu_values Debugger::dump_cpu() const noexcept
{
    return system_->cpu_.dump();
}

void Debugger::draw_framebuffer()
{
    system_->ppu_.draw_framebuffer();
}

}
