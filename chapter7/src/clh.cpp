#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

#define EPOCH 1000000

thread_local bool* pred;
thread_local bool* myNode;

class Lock {
private:
  std::atomic<bool*> tail;

public:
  Lock() : tail(new bool(false)) {}
  ~Lock() {delete tail.load();}
  void lock() {
    myNode = new bool(true);
    pred = tail.exchange(myNode);
    while (*pred)
      ;
    __sync_synchronize();
  }
  void unlock() {
    __sync_synchronize();
    *myNode = false;
    delete pred;
  }
};

// Shared
int counter = 0;
Lock *lock;

int main() {
  const size_t N = std::thread::hardware_concurrency() - 1;
  std::vector<std::thread> threads;
  std::vector<double> durations;

  lock = new Lock();

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
