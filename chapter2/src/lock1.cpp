#include <iostream>
#include <vector>
#include <array>
#include <thread>

#define FLAGUP 1
#define FLAGDOWN 0
#define NTIMES 1000

class LockOne {
  private:
    std::array<bool, 2> flags;
  public:
    void lock(int i) {
      int j = 1 - i;
      flags[i] = FLAGUP;
      std::atomic_thread_fence(std::memory_order_release);
      while(flags[j]);
    }
    void unlock(int i) {
      flags[i] = FLAGDOWN;
    }
    LockOne() : flags({FLAGDOWN, FLAGDOWN}) {}
    ~LockOne() = default;
};

int main() {
  std::vector<std::thread> threads;

  // Shared Objects
  LockOne lock;
  int count = 0;

  for(int i = 0; i < 2; i++) {
    threads.emplace_back([&lock, &count](int id){
      for(int i = 0; i < NTIMES; i++) {
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
}