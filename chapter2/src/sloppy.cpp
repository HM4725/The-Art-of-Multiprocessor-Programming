#include <iostream>
#include <vector>
#include <atomic>
#include <thread>

#define NTIMES 1000000

int main() {
  std::vector<std::thread> threads;
  double durations[2] = {0,};

  // Shared Object
  std::atomic<int> global_count(0);
  
  for(int i = 0; i < 2; i++) {
    threads.emplace_back([&global_count](int id, double durations[]){
      struct timespec cbegin, cend;
      double duration;
      int local_count = 0;
      const int sloppy_size = 10;
      clock_gettime(CLOCK_MONOTONIC, &cbegin);
      for(int i = 0; i < NTIMES; i++) {
        local_count++;
        if(local_count == sloppy_size) {
          local_count = 0;
          global_count += sloppy_size;
        }
      }
      clock_gettime(CLOCK_MONOTONIC, &cend);
      duration = (cend.tv_sec - cbegin.tv_sec) + (cend.tv_nsec - cbegin.tv_nsec) / 1000000000.0;
      durations[id] = duration;
    }, i, durations);
  }
  for(auto& thread : threads) {
    thread.join();
  }
  std::cout << "Count: " << global_count << std::endl;
  for(int i = 0; i < 2; i++) {
    std::cout << "Thread[" << i << "]'s duration: " << durations[i] << "s" << std::endl;
  }
}