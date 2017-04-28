#pragma once
#include <thread>
#include <mutex>
#include <vector>
#include <memory>
#include <functional>
#include <exception>
#include <string>
#include <atomic>
#include <algorithm>

class thread_collector
{
public:
    class exception : public std::exception
    {
    public:
        exception() = default;
        explicit exception(std::string &&message) throw()
            : m_message(std::move(message)) {}

        virtual char const* what() const throw()
        {
            return m_message.empty() ? "Unknown thread_collector exception" : m_message.c_str();
        }

    private:
        const std::string m_message;
    };//class exception

    using work = std::function<void()>;

    thread_collector() = default;

    explicit thread_collector(work &&closure)
    {
        spawn(std::move(closure));
    }

    //non-copyable
    thread_collector(const thread_collector&) = delete;
    thread_collector& operator=(const thread_collector&) = delete;

    ~thread_collector()
    {
        join();
    }

public:
    //todo: support for std::bind-like entry points
    void spawn(work &&closure)
    {
        if (closure)
        {
            std::unique_lock<std::mutex> lock(m_mutex, std::defer_lock);

            auto new_work = std::make_unique<worker>(std::move(closure));
            if (new_work->run())
            {
                //thread now spawned
                if (!new_work->finished()) //optimization: remove if atomic_bool read is slower than emplace - or for large collections
                {
                    lock.lock();
                    m_spawns.emplace_back(std::move(new_work));
                }
            }
            lazy_collect_garbage(std::move(lock));
        }
    }

    void join()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_spawns.clear();
    }

private:
    void lazy_collect_garbage(std::unique_lock<std::mutex>&& lock)
    {
        if (!lock)
            lock.lock();

        m_spawns.erase(std::remove_if(m_spawns.begin(),
                                      m_spawns.end(),
                                      [](const auto &it) { return it->finished(); }),
                       m_spawns.end());
    };

private:
    class worker
    {
    public:
        using ptr = std::unique_ptr<worker>;

        explicit worker(work &&closure)
            : m_closure(std::move(closure))
            , m_finished(false) {}

        ~worker()
        {
            if (m_thread.joinable())
                m_thread.join();
        }

    public:
        bool run()
        {
            if (m_closure)
            {
                try
                {
                    m_thread = std::thread([this]() {
                        m_closure();

                        //must not leave function objects hanging
                        m_closure = nullptr; //destruct and clear function object

                        //signal lazy garbage collector
                        m_finished = true;
                    });
                }
                catch (...)
                {
                    throw exception("unknown/unhandled exception from worker thread");
                }

                return true;
            }
            return false;
        }

        bool finished() const { return m_finished; }

    private:
        work m_closure;
        std::thread m_thread;
        std::atomic_bool m_finished;
    };//class worker

private:
    mutable std::mutex m_mutex;
    std::vector<worker::ptr> m_spawns;
};
