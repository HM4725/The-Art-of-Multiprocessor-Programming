#include <iostream>
#include <vector>
#include <thread>

#define NTIMES 1000

class LockTwo {
  private:
    volatile int victim;
  public:
    void lock(int i) {
      victim = i;
      std::atomic_thread_fence(std::memory_order_release);
      while(victim == i);
    }
    void unlock() { }
    LockTwo() : victim(0) {}
    ~LockTwo() = default;
};

int main() {
  std::vector<std::thread> threads;

  // Shared Objects
  LockTwo lock;
  int count = 0;

  for(int i = 0; i < 2; i++) {
    threads.emplace_back([&lock, &count](int id){
      for(int i = 0; i < NTIMES; i++) {
        lock.lock(id);
        count++;
        lock.unlock();
      }
    }, i);
  }
  for(auto& thread : threads) {
    thread.join();
  }
  std::cout << "Count: " << count << std::endl;
}