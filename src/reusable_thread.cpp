#include "reusable_thread.hpp"

#include <iostream>

Reusable_thread::Reusable_thread()
    : Reusable_thread(std::function<void()>())
{}

Reusable_thread::Reusable_thread(std::function<void()> fn)
    : callback_ {std::move(fn)}
{
    has_work_ = false;
    // start thread
    thread_ = std::thread([this]
    {
        while (true)
        {
            while (!has_work_)
            {
                if (exiting_)
                    return;
            }
            // scope for lock guard
            {
                const std::lock_guard<std::mutex> lock(callback_mutex_);
                if (callback_)
                    callback_();
            }
            has_work_ = false;
        }
    });
}

void Reusable_thread::set_task(std::function<void()> fn)
{
    const std::lock_guard<std::mutex> lock(callback_mutex_);
    callback_ = std::move(fn);
}

void Reusable_thread::queue()
{
    has_work_ = true;
}


