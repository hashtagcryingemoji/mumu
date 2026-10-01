#ifndef MUMU_MUTEX_H
#define MUMU_MUTEX_H

#include <atomic>
#include <cassert>
#include <concepts>
#include <optional>
#include <thread>
#include <utility>

namespace mumu {

    template<std::movable T>
    class mutex {
    public:
        class mutex_guard {
            mutex *parent_;

            explicit mutex_guard(mutex *parent) : parent_(parent) {
            }

            friend class mutex;

        public:
            mutex_guard(mutex_guard &&o) noexcept
                : parent_(std::exchange(o.parent_, nullptr)) {
            }

            mutex_guard &operator=(mutex_guard &&o) noexcept {
                if (this != &o) {
                    if (parent_) {
                        parent_->unlock();
                    }
                    parent_ = std::exchange(o.parent_, nullptr);
                }

                return *this;
            }

            T *operator->() {
                assert(parent_);
                return &parent_->content_;
            }

            const T *operator->() const {
                assert(parent_);
                return &parent_->content_;
            }

            T &operator*() {
                assert(parent_);
                return parent_->content_;
            }

            const T &operator*() const {
                assert(parent_);
                return parent_->content_;
            }

            ~mutex_guard() {
                if (parent_) parent_->unlock();
            }

            mutex_guard(const mutex_guard &o) = delete;
        };

    private:
        T content_;
        std::atomic_bool state_;

#ifndef NDEBUG
        std::atomic<std::thread::id> locker_id_{};
#endif

        void unlock() {
#ifndef NDEBUG
            locker_id_.store(std::thread::id{}, std::memory_order_relaxed);
#endif
            state_.store(false, std::memory_order_release);
        }

    public:
        explicit mutex(T content) : content_(std::move(content)), state_(false) {
        }

        [[nodiscard]] mutex_guard lock() {
#ifndef NDEBUG
            assert(locker_id_.load(std::memory_order_relaxed) != std::this_thread::get_id()); // рекурсивный захват означает дедлок
#endif
            while (state_.exchange(true, std::memory_order_acquire)) {
            }
#ifndef NDEBUG
            locker_id_.store(std::this_thread::get_id(), std::memory_order_relaxed);
#endif
            return mutex_guard(this);
        }

        [[nodiscard]] std::optional<mutex_guard> try_lock() {
            if (state_.exchange(true, std::memory_order_acquire)) {
                return std::nullopt;
            }
#ifndef NDEBUG
            locker_id_.store(std::this_thread::get_id(), std::memory_order_relaxed);
#endif
            return mutex_guard(this);
        }

        mutex(const mutex &o) = delete;

        mutex &operator=(const mutex &o) = delete;
    };

}

#endif
