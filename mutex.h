#ifndef MUMU_MUTEX_H
#define MUMU_MUTEX_H

#include <atomic>
#include <cassert>
#include <concepts>
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

    public:
        explicit mutex(T content) : content_(std::move(content)), state_(false) {
        }

        [[nodiscard]] mutex_guard lock() {
            while (state_.exchange(true, std::memory_order_acquire)) {
            }
            return mutex_guard(this);
        }

        void unlock() {
            state_.store(false, std::memory_order_release);
        }

        mutex(const mutex &o) = delete;

        mutex &operator=(const mutex &o) = delete;
    };

}

#endif
