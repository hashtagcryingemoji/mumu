#ifndef MUMU_MUTEX_H
#define MUMU_MUTEX_H

#include "atomic"

#if __cplusplus >= 202002L

#include "type_traits"
#include "concepts"

template<typename K>
concept mutexable = std::is_destructible_v<K> && std::is_object_v<K> && std::movable<K>;
#endif

namespace mumu {
    template<typename T>
#if __cplusplus >= 202002L
    requires mutexable<T>
#endif
    class mutex {
    public:
        template<typename V>
        class mutex_guard {
            V *content_;
            mutex *_parent;

            explicit mutex_guard(V *content, mutex *parent_mutex) : content_(content), _parent(parent_mutex) {
            }

            friend class mutex;

        public:
            mutex_guard(mutex_guard &&o) noexcept {
                this->content_ = o.content_;
                this->_parent = o._parent;
            }

            mutex_guard &operator=(mutex_guard &&o) noexcept {
                if (this != &o) {
                    this->content_ = o.content_;
                    this->_parent = o._parent;
                }

                return *this;
            };

            T *operator->() {
                return content_;
            }

            const T *operator->() const {
                return content_;
            }

            T &operator*() {
                return *content_;
            }

            const T &operator*() const {
                return *content_;
            }

            ~mutex_guard() {
                _parent->unlock();
            }

            mutex_guard(mutex_guard &o) = delete;
        };

    private:
        T content_;
        std::atomic_bool state_;

    public:
        explicit mutex(T content) : content_(std::move(content)), state_(false) {
        }

        mutex_guard<T> lock() {
            while (state_.exchange(true, std::memory_order_relaxed)) {
            }
            return mutex_guard(&content_, this);
        }

        void unlock() {
            state_ = false;
        }

        ~mutex() = default;

        mutex(mutex &&o) = delete;

        mutex(const mutex &o) = delete;

        mutex &operator=(mutex &&o) = delete;

        mutex &operator=(const mutex &o) = delete;
    };
}

#endif
