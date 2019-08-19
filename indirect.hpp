#pragma once

namespace gameboy
{
	
template <typename Fn>
class Indirect
{
	public:
	explicit Indirect(Fn func) : fm_ {std::move(func)} {}
	
	template <typename... Args>
	void operator()(Args &&... args) const
	{
		fn_(std::forward<Args>(args)...);
	}
	
	private:
	Fn fn_;
	
};

}