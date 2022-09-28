/* Sequential style
 */

#include <iostream>
#include <cmath>
#include <ctime>

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
  for (long n = 1; n <= limit; n++) {
    if (isPrime(n)) {
      count += 1;
    }
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
  std::cout << "Count: " << result << std::endl;
  std::cout << "Runtime: " << duration << "s" << std::endl;
  return 0;
}