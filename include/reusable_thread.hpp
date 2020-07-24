#ifndef REUSABLE_THREAD_H
#define REUSABLE_THREAD_H

#include <thread>
#include <functional>
#include <atomic>
#include <mutex>

class Reusable_thread
{
    public:
    explicit Reusable_thread();
    explicit Reusable_thread(std::function<void()> fn);

    void set_task(std::function<void()> fn);
    void queue();

    private:
    std::thread thread_;
    std::atomic<bool> has_work_ {false}, exiting_ {false};
    std::function<void()> callback_;
    std::mutex callback_mutex_;
};

#endif // REUSABLE_THREAD_H
