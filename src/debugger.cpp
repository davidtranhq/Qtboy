#include "debugger.hpp"
#include "disassembler.hpp"
#include "system.hpp"
#include "exception.hpp"

#include <sstream>
#include <iomanip>
#include <cctype>

namespace qtboy
{

Debugger::Debugger(std::shared_ptr<Gameboy> g)
    : system_ {std::move(g)}
{
    if (!system_)
        throw std::runtime_error {"Could not construct Debugger from nullptr"};
    // use update() as the debug_callback for Gameboy to be called after every CPU instruction
    system_->set_cpu_debug_callback([this]{ return cpu_callback(); });
    system_->set_memory_debug_callback([this](uint8_t b, uint16_t adr)
    {
        return memory_callback(b, adr);
    });
    // memory_map_ = system_->dump_mapped_memory();
}


Debugger::~Debugger()
{
    if (!system_)
        return;
    // unset the debug_callback
    system_->set_cpu_debug_callback({});
    system_->set_memory_debug_callback({});
    system_->set_debug_mode(false);
}

void Debugger::set_debug_mode(bool b)
{
    system_->set_debug_mode(b);
}

// CPU callback handles logging and breakpoints.
bool Debugger::cpu_callback()
{
    ++steps_;
    if (logging_)
        write_log();
    if (breaking_ && at_breakpoint())
        // return true to request a CPU break
        return true;
    return false;
}

void Debugger::memory_callback(uint8_t b, uint16_t adr)
{

}

void Debugger::run_until_break()
{
    breaking_ = true;
    system_->resume();
}

void Debugger::run_no_break()
{
    breaking_ = false;
    system_->resume();
}

void Debugger::add_breakpoint(uint16_t adr)
{
    breaks_[adr] = true;
}

void Debugger::delete_breakpoint(uint16_t adr)
{
    breaks_[adr] = false;
}

const std::unordered_map<uint16_t, bool> &Debugger::breakpoints() const
{
    return breaks_;
}

void Debugger::set_logging(bool b)
{
    logging_ = b;
}

bool Debugger::set_log_file(const std::string &path)
{
    if (log_file_) // close log file if one is already open
    {
        log_file_.close();
        log_file_.clear();
    }
    log_file_.open(path);
    return log_file_.good();
}

void Debugger::write_log()
{
    if (!log_file_.is_open())
        // use "cpu.log" as the default log file if no path is specified
        log_file_.open("cpu.log");
    log_file_ << Debugger::log() << '\n';
}

size_t Debugger::steps() const
{
    return steps_;
}

void Debugger::reset()
{
    system_->reset();
    steps_ = 0;
}


std::string Debugger::log()
{
    std::ostringstream out {};
    Cpu_dump dump {system_->dump_cpu()};
    Assembly as {disassembler_.disassemble_op(dump.next_ops, dump.pc)};
    uint8_t a = dump.af >> 8;
    uint8_t f = dump.af & 0xff;
    uint8_t b = dump.bc >> 8;
    uint8_t c = dump.bc & 0xff;
    uint8_t d = dump.de >> 8;
    uint8_t e = dump.de & 0xff;
    uint8_t h = dump.hl >> 8;
    uint8_t l = dump.hl & 0xff;
    uint8_t ly = system_->memory_read(0xff44);
    std::array<std::string, 24> flag_char // flag register in character format
    {{
        "----", "---C", "--H-", "--HC",
        "-N--", "-N-C", "-NH-", "-NHC",
        "Z---", "Z--C", "Z-H-", "Z-HC",
        "ZN--", "ZN-C", "ZNH-", "ZNHC",
    }};

    // begin output

    // register values
    out << std::uppercase << std::right << std::hex << std::setfill('0')
        << "A:" << std::setw(2) << static_cast<int>(a) << ' '
        << "F:" << flag_char[(f >> 4)] << ' '
        << "BC:" << std::setw(2) << static_cast<int>(b) << std::setw(2) << static_cast<int>(c) << ' ' << std::nouppercase
        << "DE:" << std::setw(2) << static_cast<int>(d) << std::setw(2) << static_cast<int>(e) << ' '
        << "HL:" << std::setw(2) << static_cast<int>(h) << std::setw(2) << static_cast<int>(l) << ' '
        << "SP:" << std::setw(4) << dump.sp << ' '
        << "PC:" << std::setw(4) << dump.pc << ' '
        << "LY:" << std::setw(2) << static_cast<int>(ly) << ' '
        << "IME:" << static_cast<int>(dump.ime) << ' '
        << "(cy: " << std::dec << dump.cycles << ") ";


    // instruction being executed
    out << std::uppercase << memory_map_["ROMX"].name
        << std::nouppercase << std::right << std::setfill('0') << std::hex
        << std::setw(4) << dump.pc << ": "; // PC
    std::ostringstream ops {}; // so we can set fixed width for variable
                               // number of ops
    for (uint8_t x : as.ops)
        ops << std::nouppercase << std::setfill('0') << std::hex << std::setw(2)
            << static_cast<int>(x) << ' ';
    std::string ins {as.code};
    for (auto &c : ins)
        c = static_cast<char>(std::tolower(c));
    out << std::left << std::setfill(' ') << std::setw(10) << ops.str() // bytes
        << std::setw(20) << ins; // instruction

    return out.str();
}

std::vector<Assembly> Debugger::disassemble() const
{
    const std::vector<uint8_t> &ops {dump_memory()};
    return disassembler_.disassemble(ops);
}

void Debugger::update_memory_cache() const
{
    // check to see if any changes to memory have been made
    std::vector<Memory_byte> mem_log {system_->dump_memory_log()};
    if (!mem_log.empty())
    {
        std::cout << "updating memory";
        // changes have been made, update only the changed bytes
        for (Memory_byte mb : mem_log)
        {
            if (mb.adr < 0x8000)
            {
                // this should never happen since ROM bytes won't change
                // even though you can write to it (MBC flags)
                std::ostringstream err;
                err << "ROM byte " << mb.adr << " changed when it shouldn't"
                       "have";
                throw std::runtime_error {err.str()};
            }
            else if (mb.adr < 0xa000)
            {
                uint16_t adr = mb.adr-0x8000;
                memory_map_["VRM0"].data[adr] = mb.val;
            }
            else if (mb.adr < 0xc000)
            {
                uint16_t adr = mb.adr-0xa000;
                memory_map_["RAMX"].data[adr] = mb.val;
            }
            else if (mb.adr < 0xd000)
            {
                uint16_t adr = mb.adr-0xc000;
                memory_map_["WRM0"].data[adr] = mb.val;
            }
            else if (mb.adr < 0xe000)
            {
                uint16_t adr = mb.adr-0xd000;
                memory_map_["WRMX"].data[adr] = mb.val;
            }
            else if (mb.adr < 0xfe00) // echo RAM
            {
                // this should never happen since ECHO bytes are
                // handled already
                std::ostringstream err;
                err << "ECHO ram byte " << mb.adr
                    << " changed when it shouldn't have";
                throw std::runtime_error {err.str()};
            }
            else if (mb.adr < 0xfea0)
            {
                uint16_t adr = mb.adr-0xfe00;
                memory_map_["OAM"].data[adr] = mb.val;
            }
            else if (mb.adr < 0xff00)
            {
                // undefined portion of memory
                std::ostringstream err;
                err << "Undefined byte " << mb.adr
                    << " changed when it shouldn't have";
                throw std::runtime_error {err.str()};
            }
            else if (mb.adr < 0xff80)
            {
                uint16_t adr = mb.adr-0xff00;
                memory_map_["IO"].data[adr] = mb.val;
            }
            else if (mb.adr < 0xffff)
            {
                uint16_t adr = mb.adr-0xff80;
                memory_map_["HRAM"].data[adr] = mb.val;
            }
            else if (mb.adr == 0xffff)
            {
            }
        }
    }
    // otherwise, no changes have been made so don't do anything
}

std::vector<uint8_t> Debugger::dump_memory() const
{
    // check if changes to memory have been made and update accordingly
    memory_map_ = system_->dump_mapped_memory();
    // concatenate memory map into a series of bytes
    std::vector<uint8_t> out {};
    for (const std::string &r : Memory_range_names)
        out.insert(out.end(), memory_map_[r].data.begin(),
                   memory_map_[r].data.end());
    return out;
}


std::string Debugger::dump_formatted_memory(Dump_format d) const
{
    memory_map_ = system_->dump_mapped_memory();
    std::ostringstream dump {};
    std::ostringstream out {};
    out << std::hex << std::setfill('0');
    uint16_t adr = 0;
    switch (d)
    {
        case Dump_format::Hex:
        {
            for (const std::string &r : Memory_range_names)
            {
                const std::vector<uint8_t> &v {memory_map_[r].data};
                // rows + data
                for (size_t j {0}; j < v.size(); j += 16, adr += 16)
                {
                    // row marker
                    out << std::setfill(' ') << std::setw(4)
                        << memory_map_[r].name << ':'
                        << std::setw(4) << std::setfill('0') << std::hex
                        << adr << ' ';
                    // line of 16 bytes
                    std::string line;
                    for (unsigned short k {0}; k < 16; ++k)
                    {
                        const uint8_t c {v[j+k]};
                        if (c < 32 || c == 127) // non-printable ASCII
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
            adr = 0xffff;
            // iterate backwards through memory ranges (first entry is HRAM)
            for (auto i_name {Memory_range_names.rbegin()};
                 i_name < Memory_range_names.rend();
                 ++i_name)
            {
                // get vector of bytes associated with this memory range
                const std::vector<uint8_t> &v {memory_map_[*i_name].data};
                // iterate backwards through memory (start with hi addresses)
                for (auto i_byte = v.rbegin();
                     i_byte < v.rend();
                     ++i_byte, --adr)
                {
                    std::string label = (adr == 0xffff)
                            ? "IME"
                            : memory_map_[*i_name].name;
                    out << std::setfill(' ') << std::setw(4)
                        << label << ':'
                        << std::setw(4) << std::setfill('0') << std::hex
                        << adr << ' '
                        << std::setw(2) << static_cast<int>(*i_byte)
                        << '\n';
                }
            }
        } break;
    }
    return out.str();
}

bool Debugger::at_breakpoint()
{
    uint16_t pc {system_->dump_cpu().pc};
    if (breaks_[pc])
    {
        std::cout << "Reached breakpoint " << std::hex << pc;
        return true;
    }
    return false;
}

}
