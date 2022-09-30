/* Page 10
 *
 * 1. mutual-exclusion: o (safety)
 * 2. deadlock-free: o (liveness)
 * 3. starvation-free: o (liveness)
 * 4. wait-free: x
 */

#include <iostream>
#include <vector>
#include <thread>
#include <cassert>
#include <ctime>

#define NFEEDS 1000000


// == MAIN ============================
int main() {
  std::vector<std::thread> threads;
  double durations[2] = {0,};

  // Shared Object
  bool can = 1;
  int stocks = 0;

  // Alice: Consumer
  threads.emplace_back([&can, &stocks](double durations[]){
    struct timespec cbegin, cend;
    double duration;
    clock_gettime(CLOCK_MONOTONIC, &cbegin);
    for(int i = 0; i < NFEEDS; i++) {
      while(can == 1);
      assert(can == 0);
      assert(stocks == 1);
      stocks--;
      can = 1;
    }
    clock_gettime(CLOCK_MONOTONIC, &cend);
    duration = (cend.tv_sec - cbegin.tv_sec) + (cend.tv_nsec - cbegin.tv_nsec) / 1000000000.0;
    durations[0] = duration;
  }, durations);
  // Bob: Producer
  threads.emplace_back([&can, &stocks](double durations[]){
    struct timespec cbegin, cend;
    double duration;
    clock_gettime(CLOCK_MONOTONIC, &cbegin);
    for(int i = 0; i < NFEEDS; i++) {
      while(can == 0);
      assert(can == 1);
      assert(stocks == 0);
      stocks++;
      can = 0;
    }
    clock_gettime(CLOCK_MONOTONIC, &cend);
    duration = (cend.tv_sec - cbegin.tv_sec) + (cend.tv_nsec - cbegin.tv_nsec) / 1000000000.0;
    durations[1] = duration;
  }, durations);

  for(auto& thread: threads) {
    thread.join();
  }

  assert(stocks == 0);
  std::cout << "Stocks: " << stocks << std::endl;
  std::cout << "Alice's duration: " << durations[0] << "s" << std::endl;
  std::cout << "Bob's duration  : " << durations[1] << "s" << std::endl;

  return 0;
}