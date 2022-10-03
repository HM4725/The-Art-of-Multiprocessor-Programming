#include <iostream>
#include <vector>
#include <thread>

#define NTIMES 100000

class BakeryLock {
  private:
    size_t N;
    volatile bool *flag;
    volatile int *label;
    inline bool _compare(int i, int j) {
      return label[i] > label[j] ||
        (label[i] == label[j] && i > j);
    }
    bool _conflict(int id) {
      for(int k = 0; k < N; k++) {
        if(flag[k] && _compare(id, k))
          return true;
      }
      return false;
    }
    int _max() {
      int max = 0;
      for(int i = 0; i < N; i++) {
        if(label[i] > max)
          max = label[i];
      }
      return max;
    }
  public:
    void lock(int id) {
      flag[id] = true;
      label[id] = _max() + 1;
      while(_conflict(id));
      std::atomic_thread_fence(std::memory_order_acquire);
    }
    void unlock(int id) {
      std::atomic_thread_fence(std::memory_order_release);
      flag[id] = false;
    }
    BakeryLock(size_t N) : N(N), flag(new bool[N] {false,}), label(new int[N] {0,}) {}
    ~BakeryLock() {
      delete[] flag;
      delete[] label;
    }
};

int main(void) {
  const size_t N = std::thread::hardware_concurrency();
  std::vector<std::thread> threads;
  threads.reserve(N);
  std::vector<double> durations;
  durations.resize(N, 0);

  // Shared Objects
  BakeryLock lock(N);
  int count = 0;

  for(int i = 0; i < N; i++) {
    threads.emplace_back([&lock, &count, &durations](int id) {
      struct timespec cbegin, cend;
      double duration;
      clock_gettime(CLOCK_MONOTONIC, &cbegin);
      for(int j = 0; j < NTIMES; j++) {
        lock.lock(id);
        count++;
        lock.unlock(id);
      }
      clock_gettime(CLOCK_MONOTONIC, &cend);
      duration = (cend.tv_sec - cbegin.tv_sec) + (cend.tv_nsec - cbegin.tv_nsec) / 1000000000.0;
      durations[id] = duration;
    }, i);
  }
  
  for(auto& thread : threads) {
    thread.join();
  }
  std::cout << "Count: " << count << std::endl;
  for(int i = 0; i < N; i++) {
    std::cout << "Thread[" << i << "]'s duration: " << durations[i] << "s" << std::endl;
  }
  return 0;
}