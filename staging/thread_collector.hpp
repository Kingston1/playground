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

public:
    thread_collector() noexcept
    {
    }

    template<class _Fn, class... _Args>
    explicit thread_collector(_Fn&& _Fx, _Args&&... _Ax)
    {
        spawn(std::forward<_Fn>(_Fx), std::forward<_Args>(_Ax)...);
    }

    //non-copyable
    thread_collector(const thread_collector&) = delete;
    thread_collector& operator=(const thread_collector&) = delete;

    ~thread_collector() noexcept
    {
        join();
    }

public:
    template<class _Fn, class... _Args>
    void spawn(_Fn&& _Fx, _Args&&... _Ax)
    {
        std::unique_lock<std::mutex> lock(m_mutex, std::defer_lock);

        auto work = std::make_unique<worker>(std::bind(std::forward<_Fn>(_Fx), std::forward<_Args>(_Ax)...));
        if ((*work)())
        {
            //thread now spawned
            lock.lock();
            m_spawns.push_back(std::move(work));
        }
        lazy_collect_garbage(std::move(lock));
    }

    void join() noexcept
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_spawns.clear();
    }

private:
    void lazy_collect_garbage(std::unique_lock<std::mutex>&& lock) noexcept
    {
        if (!lock)
            lock.lock();

        m_spawns.erase(std::remove_if(m_spawns.begin(),
                                      m_spawns.end(),
                                      [](const auto &work) { return work->finished(); }),
                       m_spawns.end());
    };

private:
    class worker
    {
    public:
        using ptr = std::unique_ptr<worker>;
        using work = std::function<void()>;

        explicit worker(work &&closure)
            : m_closure(std::move(closure))
            , m_finished(false) {}

        ~worker() noexcept
        {
            if (m_thread.joinable())
                m_thread.join();
        }

    public:
        bool operator()() noexcept
        {
            if (m_closure)
            {
                m_thread = std::thread([this]() {
                    try
                    {
                        m_closure();

                        //must not leave function objects hanging
                        m_closure = nullptr; //destruct and clear function object

                        //signal lazy garbage collector
                        m_finished = true;
                    }
                    catch (...)
                    {
                        throw exception("unhandled exception from worker thread");
                    }
                });

                return true;
            }
            return false;
        }

        bool finished() const noexcept
        {
            return m_finished;
        }

    private:
        work m_closure;
        std::thread m_thread;
        std::atomic_bool m_finished;
    };//class worker

private:
    mutable std::mutex m_mutex;
    std::vector<worker::ptr> m_spawns;
};
