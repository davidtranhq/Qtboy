#pragma once

#include <exception>

namespace gameboy
{
	
class Exception : std::exception
{
	public:
	Exception(const char *msg, const char *file, size_t ln);
	const char *what() const noexcept override;
	const char *file() const noexcept;
	size_t line() const noexcept;
	
	private:
	const char *msg_, *file_, *fn_;
	size_t line_;
};

class Bad_file : Exception
{
	public:
	Bad_file(const char *msg, const char *file, size_t ln);
	const char *name() const noexcept;
	
	private:
	const char *name_;
};

class Bad_disassembly : Exception
{
	public:
	Bad_disassembly(const char *msg, const char *file, size_t ln);
};
	
}