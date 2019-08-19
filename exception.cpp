#include "exception.hpp"

namespace gameboy
{
	
Exception::Exception(const char *msg, const char *file, size_t ln)
	: std::exception(), msg_ {msg}, file_ {file}, line_ {ln}
{}
	
const char *Exception::what() const noexcept
{
	return msg_;
}

const char *Exception::file() const noexcept
{
	return file_;
}

size_t Exception::line() const noexcept
{
	return line_;
}

Bad_file::Bad_file(const char *f, const char *file, size_t ln)
	: Exception("Unable to open file!", file, ln), name_ {f}
{}

const char *Bad_file::name() const noexcept
{
	return name_;
}

Bad_disassembly::Bad_disassembly(const char *msg, const char *file, size_t ln)
	: Exception(msg, file, ln)
{}

}