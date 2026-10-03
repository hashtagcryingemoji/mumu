#include <iostream>
#include <thread>
#include <vector>

#include "mutex.h"

int main() {
    mumu::mutex<int> counter{0};

    {
        std::vector<std::jthread> workers;
        for (int i = 0; i < 4; ++i) {
            workers.emplace_back([&counter] {
                for (int step = 0; step < 100000; ++step) {
                    auto guard = counter.lock();
                    ++(*guard);
                }
            });
        }
    }

    auto guard = counter.lock();
    std::cout << "counter: " << *guard << '\n';
    return 0;
}
