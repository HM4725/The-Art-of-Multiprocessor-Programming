#include <iostream>
#include <vector>
#include <thread>

#define NTIMES 1000000

class FilterLock {
  private:
    size_t N;
    std::vector<int> level;
    std::vector<int> victim;
    bool _conflict(int id) {
      for(int k = 0; k < N; k++) {
        if(k != id && level[k] >= level[id]) {
          return true;
        }
      }
      return false;
    }
  public:
    void lock(int id) {
      for(int l = 1; l < N; l++) {
        level[id] = l;
        std::atomic_thread_fence(std::memory_order_seq_cst);
        victim[l] = id;
        std::atomic_thread_fence(std::memory_order_seq_cst);
        // Wait as long as someone else is at same or
        // higher level, and I’m designated victim.
        while(_conflict(id) && victim[l] == id);
      }
      std::atomic_thread_fence(std::memory_order_acquire);
    }
    void unlock(int id) {
      std::atomic_thread_fence(std::memory_order_release);
      level[id] = 0;
    }
    FilterLock(size_t N) {
      this->N = N;
      level.resize(N, 0);
      victim.resize(N, 0);
    }
};

int main(void) {
  const size_t N = std::thread::hardware_concurrency();
  std::vector<std::thread> threads;
  threads.reserve(N);

  // Shared Objects
  FilterLock lock(N);
  int count = 0;

  for(int i = 0; i < N; i++) {
    threads.emplace_back([&lock, &count](int id) {
      for(int j = 0; j < NTIMES; j++) {
        lock.lock(id);
        count++;
        lock.unlock(id);
      }
    }, i);
  }
  
  for(auto& thread : threads) {
    thread.join();
  }
  std::cout << "Count: " << count << std::endl;
  return 0;
}