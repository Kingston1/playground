//
// thread_collector
// ~~~~~~~~~~~
// Spawn scoped threads with controlled closure lifetimes and garbage collection.
//
// Copyright (c) 2017 Mikko Saarinki
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//
#pragma once
#include <thread>
#include <mutex>
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <atomic>
#include <algorithm>

namespace staging {

class thread_collector
{
    using ulock = std::unique_lock<std::mutex>;

public:
    thread_collector() noexcept = default;

    template<class fn, class... args>
    explicit thread_collector(fn&& fx, args&&... ax)
    {
        spawn(std::forward<fn>(fx), std::forward<args>(ax)...);
    }

    ~thread_collector()
    {
        join();
    }

    thread_collector(thread_collector&& other) noexcept
    {
        move(other);
    }

    thread_collector& operator=(thread_collector&& other) noexcept
    {
        move(other);
        return (*this);
    }

    //non-copyable
    thread_collector(const thread_collector&) = delete;
    thread_collector& operator=(const thread_collector&) = delete;

    void swap(thread_collector& other) noexcept
    {
        const auto locks = acquire_locks(other);
        std::swap(m_spawns, other.m_spawns);
    }

public:
    template<class fn, class... args>
    void spawn(fn&& fx, args&&... ax)
    {
        ulock lock(m_mutex, std::defer_lock);

        auto closure = std::bind(std::forward<fn>(fx), std::forward<args>(ax)...);
        auto work = std::make_unique<worker>(std::move(closure));
        if ((*work)())
        {
            //thread now spawned
            lock.lock();
            m_spawns.push_back(std::move(work));
        }
        lazy_collect_garbage(std::move(lock));
    }

    void join()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_spawns.clear();
    }

private:
    std::pair<ulock, ulock> acquire_locks(thread_collector& other) const noexcept
    {
        auto locks = std::make_pair(
            ulock(m_mutex, std::defer_lock),
            ulock(other.m_mutex, std::defer_lock));

        std::lock(locks.first, locks.second);//lock both unique_locks without deadlock
        return locks;//copy elision
    }

    void move(thread_collector& other) noexcept
    {
        const auto locks = acquire_locks(other);
        m_spawns = std::move(other.m_spawns);
    }

    void lazy_collect_garbage(ulock&& lock) noexcept
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

        explicit worker(work &&closure) noexcept : m_closure(std::move(closure)) {}

        ~worker()
        {
            if (m_thread.joinable())
                m_thread.join();
        }

    public:
        bool operator()()
        {
            if (!m_closure)
                return false;

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
                    //uncaught exceptions from closures are ill-formed
                    std::terminate();
                }
            });
            return true;
        }

        bool finished() const noexcept
        {
            return m_finished;
        }

    private:
        work m_closure;
        std::thread m_thread;
        std::atomic_bool m_finished = false;
    };//class worker

private:
    mutable std::mutex m_mutex;
    std::vector<worker::ptr> m_spawns;
};

}//namespace staging
