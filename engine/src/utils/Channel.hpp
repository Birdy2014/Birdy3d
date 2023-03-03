#pragma once

#include <condition_variable>
#include <optional>
#include <queue>

template <typename T>
class Channel {
public:
    std::optional<T> try_get()
    {
        // unique_lock can be locked and unlocked
        std::lock_guard<std::mutex> queue_lock{m_queue_mutex};

        if (!m_queue.empty()) {
            auto event = std::move(m_queue.front());
            m_queue.pop();

            return event;
        }

        return {};
    }

    T get()
    {
        while (true) {
            // unique_lock can be locked and unlocked
            std::unique_lock<std::mutex> queue_lock{m_queue_mutex};

            // condition_variable waits for condition and locks lock when executing predicate
            m_convar.wait(queue_lock, [&]() { return !m_queue.empty(); });

            if (!m_queue.empty()) {
                auto event = std::move(m_queue.front());
                m_queue.pop();

                return event;
            }
        }
    }

    void push_task(T item)
    {
        {
            // lock_guard locks mutex in this scope
            std::lock_guard<std::mutex> lock{m_queue_mutex};
            m_queue.push(std::move(item));
        }

        m_convar.notify_one();
    }

private:
    std::queue<T> m_queue;
    std::mutex m_queue_mutex;
    std::condition_variable m_convar;
};
