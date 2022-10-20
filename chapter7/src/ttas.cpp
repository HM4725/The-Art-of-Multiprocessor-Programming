#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

#define EPOCH 1000000

class Lock {
private:
  bool state;

public:
  Lock() : state(false) {}
  void lock() {
    while (true) {
      while (state)
        ;
      if (!__sync_lock_test_and_set(&state, true)) {
        return;
      }
    }
    __sync_synchronize();
  }
  void unlock() {
    __sync_synchronize();
    state = false; 
  }
};

// Shared
int counter = 0;
Lock lock;

int main() {
  const size_t N = std::thread::hardware_concurrency() - 1;
  std::vector<std::thread> threads;
  std::vector<double> durations;

  threads.reserve(N);
  durations.resize(N, 0);

  for (int i = 0; i < N; i++) {
    threads.emplace_back(
        [&durations](int id) {
          struct timespec cbegin, cend;
          double duration;

          clock_gettime(CLOCK_MONOTONIC, &cbegin);
          for (int seq = 0; seq < EPOCH; seq++) {
            lock.lock();
            counter++;
            lock.unlock();
          }
          clock_gettime(CLOCK_MONOTONIC, &cend);

          duration = (cend.tv_sec - cbegin.tv_sec) +
                     (cend.tv_nsec - cbegin.tv_nsec) / 1000000000.0;
          durations[id] = duration;
        },
        i);
  }
  for (auto &th : threads) {
    th.join();
  }
  std::cout << "Finish: " << counter << std::endl;
  for (int i = 0; i < N; i++) {
    std::cout << "Thread[" << i << "]'s duration: " << durations[i] << "s"
              << std::endl;
  }
}
