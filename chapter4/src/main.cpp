#include <iostream>
#include <cstdlib>
#include <vector>
#include <thread>
#include <chrono>
#include <future>
#include <ctime>
#include "globals.hpp"
#include "wait_free_snapshot.hpp"


static void usage() {
  std::cerr << "Usage: run <number-of-worker-threads>" << std::endl;
}

// Thread
void worker(int id, std::promise<long> p) {
  long counter = 0;
  thread::thread_id = id;
  while(!shared::finish) {
    shared::snapshot->update(std::rand());
    counter++;
  }
  p.set_value(counter);
}

////////////////////////////////////////////////
//                     MAIN                   //
////////////////////////////////////////////////
int main(int argc, char **argv) {
  // Parse the command line parameter.
  if(argc != 2) {
    usage();
    exit(EXIT_FAILURE);
  }
  const int N = atoi(argv[1]);
  if(N < 1) {
    usage();
    exit(EXIT_FAILURE);
  }

  // Main Variables
  std::vector<std::thread> threads;
  std::vector<std::future<long>> futures;
  long total = 0;
  threads.reserve(N);
  futures.reserve(N);
  std::srand(time(NULL));

  // Init Shared Object
  shared::snapshot = new WaitFreeSnapshot<int>(N);
  shared::finish = false;

  // Main Routine
  for(int i = 0; i < N; i++) {
    std::promise<long> p;
    futures.emplace_back(p.get_future());
    threads.emplace_back(worker, i, std::move(p));
  }
  std::this_thread::sleep_for(std::chrono::seconds(60));
  shared::finish = true;
  for(auto& future: futures) {
    total += future.get();
  }
  for(auto& thread: threads) {
    thread.join();
  }
  std::cout << total << std::endl;
  delete shared::snapshot;
  exit(EXIT_SUCCESS);
}
