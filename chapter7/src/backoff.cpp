#include <atomic>
#include <chrono>
#include <ctime>
#include <iostream>
#include <thread>
#include <vector>

#define EPOCH 1000000

class Backoff {
private:
  const int MIN_DELAY;
  const int MAX_DELAY;
  int limit;
  static int __random(int min, int max) {
    return min + rand() % ((max + 1) - min);
  }

public:
  Backoff(int min, int max) : MIN_DELAY(min), MAX_DELAY(max) { limit = min; }
  void backoff() {
    int delay = __random(limit / 2, limit);
    limit = std::max(MAX_DELAY, 2 * limit);
    std::this_thread::sleep_for(std::chrono::microseconds(delay));
  }
};

class Lock {
private:
  bool state;
  static constexpr int MIN_DELAY = 1;
  static constexpr int MAX_DELAY = 600;

public:
  Lock() : state(false) {}
  void lock() {
    Backoff backoff(MIN_DELAY, MAX_DELAY);
    while (true) {
      while (state)
        ;
      if (!__sync_lock_test_and_set(&state, true)) {
        __sync_synchronize();
        return;
      } else {
        backoff.backoff();
      }
    }
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
  srand(time(NULL));

  const size_t N = std::thread::hardware_concurrency() - 1;
  // const size_t N = 1;
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
  // std::cout << call_backoff << std::endl;
}
