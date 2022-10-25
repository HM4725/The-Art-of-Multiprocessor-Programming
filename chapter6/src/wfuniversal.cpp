#include "sequential.hpp"
#include "universal.hpp"
#include "method.hpp"
#include <iostream>
#include <thread>
#include <vector>

#define EPOCH 2000

int main() {
  const size_t N = std::thread::hardware_concurrency() - 1;
  std::vector<std::thread> threads;
  WFUniversal<int, SeqQueue<int>> universal(N);
  std::atomic<int> order(0);

  for (int i = 0; i < N; i++) {
    threads.emplace_back(std::thread(
        // Concurrent Push
        [&universal, &order](int id) {
          for (int l = 0; l < EPOCH; l++) {
            universal.apply(id, Invoc<int>("push", ++order));
          }
        },
        i));
  }
  for (auto &thread : threads) {
    thread.join();
  }
  threads.clear();
  std::cout << "push end" << std::endl;

  for (int i = 0; i < N; i++) {
    threads.emplace_back(std::thread(
        [&universal, &order](int id) {
          for (int l = 0; l < EPOCH; l++) {
            //std::cout << universal.apply(id, Invoc<int>("pop")).v << " ";
            universal.apply(id, Invoc<int>("pop"));
          }
        },
        i));
  }
  for (auto &thread : threads) {
    thread.join();
  }
  std::cout << "pop end" << std::endl;
  return 0;
}