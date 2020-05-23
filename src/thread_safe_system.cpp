#include "thread_safe_system.hpp"

namespace gameboy
{

void Thread_safe_system::run_concurrently()
{
}

System &Thread_safe_system::system()
{
    return system_;
}

std::mutex &Thread_safe_system::mutex()
{
    return mutex_;
}

}
