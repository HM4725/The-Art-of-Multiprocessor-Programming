#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
#include <numeric>

#define EPOCH 1000000

int main() {
  const size_t N = std::thread::hardware_concurrency() - 1;
  std::vector<std::thread> threads;
  std::vector<double> durations;
  std::vector<int> results;

  threads.reserve(N);
  durations.resize(N, 0);
  results.resize(N, 0);

  for (int i = 0; i < N; i++) {
    threads.emplace_back(
        [&durations, &results](int id) {
          struct timespec cbegin, cend;
          double duration;
          int counter = 0;

          clock_gettime(CLOCK_MONOTONIC, &cbegin);
          for (int seq = 0; seq < EPOCH; seq++) {
            counter++;
            __sync_synchronize();
          }
          clock_gettime(CLOCK_MONOTONIC, &cend);

          duration = (cend.tv_sec - cbegin.tv_sec) +
                     (cend.tv_nsec - cbegin.tv_nsec) / 1000000000.0;
          durations[id] = duration;
          results[id] = counter;
        },
        i);
  }
  for (auto &th : threads) {
    th.join();
  }
  int sum = std::accumulate(results.begin(), results.end(), 0);
  std::cout << "Finish: " << sum << std::endl;
  for (int i = 0; i < N; i++) {
    std::cout << "Thread[" << i << "]'s duration: " << durations[i] << "s"
              << std::endl;
  }
}
