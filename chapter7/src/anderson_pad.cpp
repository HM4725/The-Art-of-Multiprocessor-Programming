#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

#define EPOCH 1000000
#define __CACHE_LINE_SIZE 128


thread_local int myslot;

struct alignas(__CACHE_LINE_SIZE) padFlag {
  bool flag;
};

class Lock {
private:
  padFlag *flags;
  const int N;
  std::atomic<int> next;

public:
  Lock() = delete;
  Lock(int N) : flags(new padFlag[N]), N(N), next(0) {
    flags[0].flag = true;
    for(int i = 1; i < N; i++) {
      flags[i].flag = false;
    }
  }
  ~Lock() { delete[] flags; }
  void lock() {
    myslot = next++;
    while (!flags[myslot % N].flag)
      ;
    flags[myslot % N].flag = false;
    __sync_synchronize();
  }
  void unlock() {
    __sync_synchronize();
    flags[(myslot + 1) % N].flag = true;
  }
};

// Shared
int counter = 0;
Lock *lock;

int main() {
  const size_t N = std::thread::hardware_concurrency() - 1;
  std::vector<std::thread> threads;
  std::vector<double> durations;

  lock = new Lock(N);

  threads.reserve(N);
  durations.resize(N, 0);

  for (int i = 0; i < N; i++) {
    threads.emplace_back(
        [&durations](int id) {
          struct timespec cbegin, cend;
          double duration;

          clock_gettime(CLOCK_MONOTONIC, &cbegin);
          for (int seq = 0; seq < EPOCH; seq++) {
            lock->lock();
            counter++;
            lock->unlock();
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
  delete lock;
  std::cout << "Finish: " << counter << std::endl;
  for (int i = 0; i < N; i++) {
    std::cout << "Thread[" << i << "]'s duration: " << durations[i] << "s"
              << std::endl;
  }
}
