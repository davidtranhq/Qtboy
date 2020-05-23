#include "debugger.hpp"
#include "disassembler.hpp"
#include "system.hpp"
#include "exception.hpp"

#include <sstream>
#include <iomanip>
#include <cctype>

namespace gameboy
{

Debugger::Debugger(System *s)
    : system_ {s}
{
    if (!system_)
        throw Bad_debugger {"Could not attach system to debugger",
                            __FILE__, __LINE__};
    system_->pause();
    system_->set_step_callback([this]{ return this->step_callback(); });
    memory_map_ = system_->dump_memory();
}

void Debugger::run()
{
    system_->run_concurrently();
}

void Debugger::run_until_break()
{
}

void Debugger::pause()
{
    system_->pause();
}

void Debugger::step(size_t n)
{
    system_->pause();
    for (size_t i {0}; i < n; ++i)
    {
        system_->step(1);
    }
}

void Debugger::reset()
{
    system_->reset();
    steps_ = 0;
}

void Debugger::add_breakpoint(uint16_t adr)
{
    breaks_[adr] = true;
}

void Debugger::delete_breakpoint(uint16_t adr)
{
    breaks_[adr] = false;
}

void Debugger::enable_logging(bool b)
{
    log_ = b;
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
        log_file_.open("cpu.log");
    log_file_ << Debugger::log() << '\n';
}

std::string Debugger::log()
{
    std::ostringstream out {};
    Cpu_values dump {system_->dump_cpu()};
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
        c = std::tolower(c);
    out << std::left << std::setfill(' ') << std::setw(10) << ops.str() // bytes
        << std::setw(20) << ins; // instruction

    return out.str();
}

size_t Debugger::steps() const
{
    return steps_;
}

void Debugger::update_memory_cache() const
{
    std::vector<Memory_byte> mem_log {system_->dump_memory_log()};
    if (!mem_log.empty())
    {
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
}

std::vector<uint8_t> Debugger::dump_memory() const
{
    update_memory_cache();
    std::vector<uint8_t> out {};
    for (const std::string &r : Memory_range_names)
        out.insert(out.end(), memory_map_[r].data.begin(),
                   memory_map_[r].data.end());
    return out;
}

std::unordered_map<std::string, Memory_range> Debugger::dump_mapped_memory() const
{
    // create echo ram of 0xc00-0xdff  (WRM0 and WRMX)
    std::vector<uint8_t> echo_ram {memory_map_["WRM0"].data};
    echo_ram.insert(echo_ram.end(), memory_map_["WRMX"].data.begin(),
                    memory_map_["WRMX"].data.begin() + 0x0e00);

    // add extra areas to the memory map
    memory_map_["ECHO"] = {"ECHO", echo_ram};
    memory_map_["XXXX"] = {"UNUS", std::vector<uint8_t>(0x60)};
    return memory_map_;
}

std::string Debugger::dump_formatted_memory(Dump_format d) const
{
    update_memory_cache();
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
                const std::vector<uint8_t> &v {memory_map_[r].data};
                // rows + data
                for (size_t j {0}; j < v.size(); j += 16, i += 16)
                {
                    // row marker
                    out << std::setfill(' ') << std::setw(4)
                        << memory_map_[r].name << ':'
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
                const std::vector<uint8_t> &v {memory_map_[*n].data};
                for (auto j {v.rbegin()}; j != v.rend(); ++j, --i)
                {
                    out << std::setfill(' ') << std::setw(4)
                        << memory_map_[*n].name << ':'
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

std::vector<uint8_t> Debugger::dump_rom() const noexcept
{
    return system_->dump_rom();
}

Cpu_values Debugger::dump_cpu() const noexcept
{
    return system_->dump_cpu();
}

std::array<Texture, 384> Debugger::dump_tileset()
{
    return system_->dump_tileset();
}

Texture Debugger::dump_background()
{
    return system_->dump_background();
}

Texture Debugger::dump_window()
{
    return system_->dump_window();
}


std::array<Sprite, 40> Debugger::dump_sprites()
{
    return system_->dump_sprites();
}

std::string Debugger::hex_dump() const
{
    const std::vector<uint8_t> &ops {dump_memory()};
    uint8_t len {0};
    std::ostringstream out {};
    for (uint32_t pc {0}; pc < ops.size(); pc += len)
    {
        Instruction ins {instructions[ops[pc]]};
        len = ins.length;
        std::ostringstream bytes {};
        for (uint8_t i {0}; i < ins.length; ++i) // opcodes
            bytes << std::setfill('0') << ' ' << std::hex
                  << std::setw(2) << static_cast<int>(ops[pc+i]);
        const std::string operand1 {ins.operand1.empty() ? "" : ins.operand1};
        const std::string operand2 {ins.operand2.empty() ? "" : ins.operand2};
        out << std::setfill('0') << std::setw(4) << std::hex << std::right << pc; // address
        out << std::setfill(' ') << std::setw(10) << std::left << bytes.str() // opcodes
            << ' ' << std::setw(4) << ins.name; // instruction
        if (!operand1.empty())
        {
            out << ' ' << operand1;
            if (!operand2.empty())
                out << ',' << operand2;
        }
        out << '\n';
    }
    return out.str();
}

class Hex // for more convenient formatting
{
    public:
    explicit Hex(int x) : x_ {x} {}
    friend std::ostream &operator<<(std::ostream &os, const Hex &h)
    {
        os << std::right << std::hex << std::setfill('0')
           << std::uppercase << std::setw(2) << h.x_;
        return os;
    }

    private:
    int x_;
};

Assembly Debugger::disassemble_op() const
{
    return disassemble_op(dump_cpu().pc);
}

Assembly Debugger::disassemble_op(uint16_t adr) const
{
    const std::array<uint8_t, 3> &ops
    {{
        system_->memory_read(adr),
        system_->memory_read(adr+1),
        system_->memory_read(adr+2)
    }};
    Instruction ins;
    if (ops[0] == 0xcb)
        ins = cb_instructions[ops[1]];
    else
        ins = instructions[ops[0]];
    std::ostringstream code {};
    const std::string operand1 {ins.operand1.empty() ? "" : ins.operand1};
    const std::string operand2 {ins.operand2.empty() ? "" : ins.operand2};
    code << std::setfill(' ') << std::left << std::setw(4) << ins.name; // instruction
    if (!operand1.empty())
    {
        code << ' ' << parse_operand(ops, operand1, adr);
        if (!operand2.empty())
        {
            code << ',' << parse_operand(ops, operand2, adr);
        }
    }
    std::vector<uint8_t> ops_used(ins.length);
    for (uint8_t i {0}; i < ops.size(); ++i)
        ops_used[i] = ops[i];
    return Assembly {ops_used, ins, code.str()};
}

std::vector<Assembly> Debugger::disassemble() const
{
    const std::vector<uint8_t> &ops {dump_memory()};
    std::vector<Assembly> out {};
    for (uint16_t i {0}; i < ops.size(); i += instructions[ops[i]].length)
    {
        out.push_back(disassemble_op(i));
    }
    return out;
}

void Debugger::step_callback()
{
    if (at_breakpoint())
        system_->pause();
    if (log_)
        write_log();
}

bool Debugger::at_breakpoint()
{
    uint16_t pc {dump_cpu().pc};
    if (breaks_[pc])
        return true;
    return false;
}

std::string Debugger::parse_operand(const std::array<uint8_t, 3> &ops,
                                    const std::string &operand,
                                    uint16_t adr) const
{
    std::ostringstream out {};
    if (operand == "a16" || operand == "d16") // 16-bit immediate
        out << '#' << Hex(ops[2]) << Hex(ops[1]) << 'h';
    else if (operand == "r8") // 8-bit signed immediate
        out << '#' << Hex(static_cast<int8_t>(adr+ops[1])) << 'h';
    else if (operand == "d8") // 8-bit unsigned immediate
        out << '#' << Hex(ops[1]) << 'h';
    else if (operand == "(a8)") // 8-bit unsigned indexing
        out << "(#" << Hex(ops[1]) << "h)";
    else if (operand == "(a16)" || operand == "(d16)") // 16-bit direct
        out << "(#" << Hex(ops[2]) << Hex(ops[1]) << "h)";
    else
        out << operand;
    return out.str();
}

}
