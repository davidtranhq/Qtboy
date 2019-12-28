#pragma once

#include <exception>

namespace gameboy
{
	
class Exception : public std::exception
{
	public:
    Exception(const char *msg, const char *file, size_t ln);
    const char *what() const noexcept override;
    const char *file() const noexcept { return file_; }
    size_t line() const noexcept { return line_; }
	
	private:
    const char *msg_, *file_;
	size_t line_;
};

class Bad_file : public Exception
{
	public:
    Bad_file(const char *msg, const char *file, size_t ln);
    const char *name() const noexcept { return name_; }
	
	private:
	const char *name_;
};

class Bad_disassembly : public Exception
{
	public:
    Bad_disassembly(const char *msg, const char *file, size_t ln);
};

class Bad_memory : public Exception
{
    public:
    Bad_memory(const char *msg, const char *file, size_t ln);
};

};
