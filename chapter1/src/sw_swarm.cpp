/* figure1
 * march version
 * Compare 31st line with figure1's 30th line.
 */

#include <iostream>
#include <vector>
#include <thread>
#include <cmath>
#include <ctime>

#define NTHREADS 8

bool isPrime(long n) {
  for (long i = 2; i<=(long)sqrt(n); i++) {
      if (n % i == 0) {
      return false;
    }
  }
  return true;
}

long countPrime(long limit) {
  long count = 0;
  if(limit < 1) {
    count = 0;
  } else if(limit == 1) {
    count = 1;
  } else if(limit == 2) {
    count = 2;
  } else if(limit > 2) {
    std::vector<std::thread> threads;
    long results[NTHREADS] = {0,};
    threads.reserve(NTHREADS);
    for (int i = 0; i < NTHREADS; i++) {
      threads.emplace_back([&results, limit](int id){
        long count = 0;
        // Check only odd numbers concurrently.
        for (int n = 2 * id + 1; n <= limit; n += NTHREADS * 2) {
          if (isPrime(n)) {
            count += 1;
          }
        }
        results[id] = count;
      }, i);
    }
    for (auto& thread: threads) {
      thread.join();
    }
    for (int i = 0; i < NTHREADS; i++) {
      count += results[i];
    }
    count += 1; // 2 is prime too.
  }
  return count;
}

int main() {
  struct timespec cbegin, cend;
  double duration;
  long result;

  clock_gettime(CLOCK_MONOTONIC, &cbegin);
  result = countPrime((long)pow(10, 7));
  clock_gettime(CLOCK_MONOTONIC, &cend);
  duration = (cend.tv_sec - cbegin.tv_sec) + (cend.tv_nsec - cbegin.tv_nsec) / 1000000000.0;
  std::cout << "Result: " << result << std::endl;
  std::cout << "Runtime: " << duration << "s" << std::endl;
  return 0;
}