#include "CoarseGrainedSet.h"
#include "FineGrainedSet.h"
#include "OptimisticSet.h"
#include "LazySyncSet.h"
#include "LockFreeSet.h"
#include "types.h"
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#ifndef DATASTRUCTURE
  #define DATASTRUCTURE CoarseGrainedSet
#endif

#define NTHREADS (8)
#define NWORKLOADS (80000)
#define QUOTA (NWORKLOADS / NTHREADS)

struct workdata {
  char type;
  int64 key;
};

workdata parseWorkdata(std::string l) {
  workdata w;
  w.type = l[0];
  w.key = static_cast<int64>(std::stoi(l.substr(2)));
  return w;
}

int main() {
  std::vector<workdata> Workloads[NTHREADS];
  std::vector<std::thread> threads;
  DATASTRUCTURE Set;

  // Workload init
  std::ifstream fworkload("workload.txt");
  if (!fworkload.is_open()) {
    return -1;
  }
  std::string line;
  int nline = 0;
  while (getline(fworkload, line)) {
    Workloads[nline / QUOTA].push_back(parseWorkdata(line));
    nline += 1;
  }

  // Main routine
  auto ThreadFunc = [&Workloads, &Set](int id) {
    std::vector<workdata> &workload = Workloads[id];
    std::vector<workdata>::iterator itr;
    for (itr = workload.begin(); itr != workload.end(); itr++) {
      char type = itr->type;
      int64 key = itr->key;
      switch (type) {
      case 'A':
        Set.add(key);
        break;
      case 'R':
        Set.remove(key);
        break;
      case 'C':
        Set.contains(key);
        break;
      default:
        break;
      }
    }
  };

  for (int i = 0; i < NTHREADS; i++) {
    threads.push_back(std::thread(ThreadFunc, i));
  }
  for (auto &thread : threads) {
    thread.join();
  }

  // Validation
  std::ifstream fresult("result.txt");
  if (!fresult.is_open()) {
    return -1;
  }
  getline(fresult, line);
  int sz= std::stoi(line);
  std::string res = Set.size() == sz ? "correct" : "incorrect";
  std::cout << res << std::endl;

  return 0;
}