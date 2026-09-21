#include <gtest/gtest.h>
#include <thread>

#include "../mutex.h"

class MutexIntTest : public testing::Test {
protected:
    mumu::mutex<int> m{0};
};

TEST_F(MutexIntTest, HandlesSingleThread) {
    auto l = m.lock();
    (*l)++;
    (*l)++;
    (*l)++;

    EXPECT_EQ(*l, 3);
}

TEST_F(MutexIntTest, HandlesDestructFromScope) {
    {
        auto l = m.lock();
        (*l)++;
        (*l)++;
        (*l)++;
    }

    {
        auto l = m.lock();
        (*l)++;
        (*l)++;
        (*l)++;
    }

    auto l = m.lock();

    EXPECT_EQ(*l, 6);
}

TEST_F(MutexIntTest, HandlesTryLock) {
    auto l1 = m.try_lock();
    auto l2 = m.try_lock();

    EXPECT_TRUE(l1.has_value());
    EXPECT_FALSE(l2.has_value());
}

TEST_F(MutexIntTest, HandlesMultithreaded) {
    std::thread t1{
        [this] {
            for (int i = 0; i < 10000; i++) {
                auto l = m.lock();
                (*l)++;
            }
        }
    };

    std::thread t2{
        [this] {
            for (int i = 0; i < 10000; i++) {
                auto l = m.lock();
                (*l)++;
            }
        }
    };

    t1.join();
    t2.join();

    auto l = m.lock();

    ASSERT_EQ(*l, 20000);
}

TEST(Mutex, MoveGuardTransfersOwnership) {
    mumu::mutex m{0};
    {
        auto l = m.lock();
        auto l2 = std::move(l);
        (*l2)++;
        ASSERT_EQ(*l2, 1);
    }

    auto l = m.try_lock();
    EXPECT_TRUE(l.has_value());
}
