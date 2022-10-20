#include <atomic>
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

#define EPOCH 1000000

struct Qnode {
  bool locked;
  Qnode *next;
  Qnode(bool state) : locked(state), next(nullptr) {}
};

thread_local Qnode *myNode = nullptr;

class Lock {
private:
  std::atomic<Qnode *> tail;
  std::atomic<int> count;

public:
  Lock() : tail(nullptr), count(0) {}
  ~Lock() {}
  void lock() {
    if(myNode == nullptr) {
      myNode = new Qnode(true);
    } else {
      myNode->locked = true;
      myNode->next = nullptr;
    }
    Qnode *pred = tail.exchange(myNode);
    if (pred != nullptr) {
      pred->next = myNode;
      while (myNode->locked)
        ;
    }
    __sync_synchronize();
  }
  void unlock() {
    __sync_synchronize();
    if (myNode->next == nullptr) {
      Qnode *expected = myNode;
      if (tail.compare_exchange_strong(expected, nullptr)) {
        return;
      }
      while (myNode->next == nullptr)
        ;
    }
    myNode->next->locked = false;
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
