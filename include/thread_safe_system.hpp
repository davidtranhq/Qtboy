#ifndef THREAD_SAFE_SYSTEM_HPP
#define THREAD_SAFE_SYSTEM_HPP

#include "system.hpp"
#include <mutex>

namespace gameboy
{

class Thread_safe_system
{
    public:
    void run_concurrently();
    System &system();
    std::mutex &mutex();

    private:
    System system_ {};
    std::mutex mutex_;
    std::atomic<bool> running_ {false};
};

}

#endif // THREAD_SAFE_SYSTEM_HPP
