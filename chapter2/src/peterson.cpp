#include <iostream>
#include <vector>
#include <array>
#include <thread>

#define FLAGUP 1
#define FLAGDOWN 0
#define NTIMES 1000000

class PetersonLock {
  private:
    std::array<bool, 2> flags;
    volatile int victim;
  public:
    void lock(int i) {
      int j = 1 - i;
      flags[i] = FLAGUP;
      victim = i;
      __sync_synchronize();
      while(flags[j] && victim == i);
    }
    void unlock(int i) {
      __sync_synchronize();
      flags[i] = FLAGDOWN;
    }
    PetersonLock() : flags({FLAGDOWN, FLAGDOWN}), victim(0) {}
    ~PetersonLock() = default;
};

int main() {
  std::vector<std::thread> threads;
  double durations[2] = {0,};

  // Shared Objects
  PetersonLock lock;
  int count = 0;

  for(int i = 0; i < 2; i++) {
    threads.emplace_back([&lock, &count](int id, double durations[]){
      struct timespec cbegin, cend;
      double duration;
      clock_gettime(CLOCK_MONOTONIC, &cbegin);
      for(int i = 0; i < NTIMES; i++) {
        lock.lock(id);
        count++;
        lock.unlock(id);
      }
      clock_gettime(CLOCK_MONOTONIC, &cend);
      duration = (cend.tv_sec - cbegin.tv_sec) + (cend.tv_nsec - cbegin.tv_nsec) / 1000000000.0;
      durations[id] = duration;
    }, i, durations);
  }
  for(auto& thread : threads) {
    thread.join();
  }
  std::cout << "Count: " << count << std::endl;
  for(int i = 0; i < 2; i++) {
    std::cout << "Thread[" << i << "]'s duration: " << durations[i] << "s" << std::endl;
  }
}