/* Page 7, Figure 1.4
 *
 * 1. mutual-exclusion: o
 * 2. dead-lock freedom: x
 */

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

#define NFEEDS 1000000


// == MAIN ============================
int main() {
  std::vector<std::thread> threads;
  std::mutex mtx;
  double durations[2] = {0,};

  // Shared Object
  int pond = 0;

  // Alice
  threads.emplace_back([&mtx, &pond](double durations[]){
    struct timespec cbegin, cend;
    double duration;
    clock_gettime(CLOCK_MONOTONIC, &cbegin);
    for(int i = 0; i < NFEEDS; i++) {
      mtx.lock();
      pond++;
      mtx.unlock();
    }
    clock_gettime(CLOCK_MONOTONIC, &cend);
    duration = (cend.tv_sec - cbegin.tv_sec) + (cend.tv_nsec - cbegin.tv_nsec) / 1000000000.0;
    durations[0] = duration;
  }, durations);
  // Bob
  threads.emplace_back([&mtx, &pond](double durations[]){
    struct timespec cbegin, cend;
    double duration;
    clock_gettime(CLOCK_MONOTONIC, &cbegin);
    for(int i = 0; i < NFEEDS; i++) {
      mtx.lock();
      pond++;
      mtx.unlock();
    }
    clock_gettime(CLOCK_MONOTONIC, &cend);
    duration = (cend.tv_sec - cbegin.tv_sec) + (cend.tv_nsec - cbegin.tv_nsec) / 1000000000.0;
    durations[1] = duration;
  }, durations);

  for(auto& thread: threads) {
    thread.join();
  }
  std::cout << "Used: " << pond << std::endl;
  std::cout << "Alice's duration: " << durations[0] << "s" << std::endl;
  std::cout << "Bob's duration  : " << durations[1] << "s" << std::endl;

  return 0;
}