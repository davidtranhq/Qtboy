#include "exception.hpp"

namespace gameboy
{

Exception::Exception(const char *msg, const char *file, size_t ln)
    : msg_ {msg}, file_ {file}, line_ {ln}
{}

const char *Exception::what() const noexcept
{
    return msg_;
}

Bad_file::Bad_file(const char *msg, const char *file, size_t ln)
    : Exception(msg, file, ln), name_ {file}
{}

Bad_disassembly::Bad_disassembly(const char *msg, const char *file, size_t ln)
    : Exception(msg, file, ln)
{}

Bad_memory::Bad_memory(const char *msg, const char *file, size_t ln)
    : Exception(msg, file, ln)
{}



}
