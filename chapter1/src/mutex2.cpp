/* Page 7
 *
 * 1. mutual-exclusion: o
 * 2. deadlock-free: o
 * 3. starvation-free: x
 * 4. wait-free: x
 */

#include <iostream>
#include <vector>
#include <thread>
#include <cassert>
#include <ctime>

#define NFEEDS 1000000

class BobProtocol;

class AliceProtocol {
  private:
    volatile bool flag;
    friend class BobProtocol;
  public:
    void lock(BobProtocol& bob);
    void unlock();
    AliceProtocol() : flag(0) {}
};

class BobProtocol {
  private:
    volatile bool flag;
    friend class AliceProtocol;
  public:
    void lock(AliceProtocol& alice);
    void unlock();
    BobProtocol() : flag(0) {}
};

void AliceProtocol::lock(BobProtocol& bob) {
  flag = 1;
  __sync_synchronize();
  while(bob.flag);
}

void AliceProtocol::unlock() {
  flag = 0;
}

void BobProtocol::lock(AliceProtocol& alice) {
  flag = 1;
  __sync_synchronize();
  while(alice.flag) {
    flag = 0;
    while(alice.flag);
    flag = 1;
    __sync_synchronize();
  }
}

void BobProtocol::unlock() {
  flag = 0;
}

enum Occupant { ALICE, BOB };

// == MAIN ============================
int main() {
  std::vector<std::thread> threads;
  AliceProtocol alice;
  BobProtocol bob;
  enum Occupant occupant = ALICE;
  double durations[2] = {0,};

  // Shared Object
  int pond = 0;

  // Alice
  threads.emplace_back([&alice, &bob, &pond, &occupant](double durations[]){
    struct timespec cbegin, cend;
    double duration;
    clock_gettime(CLOCK_MONOTONIC, &cbegin);
    for(int i = 0; i < NFEEDS; i++) {
      alice.lock(bob);
      occupant = ALICE;
      pond++;
      assert(occupant == ALICE);
      alice.unlock();
    }
    clock_gettime(CLOCK_MONOTONIC, &cend);
    duration = (cend.tv_sec - cbegin.tv_sec) + (cend.tv_nsec - cbegin.tv_nsec) / 1000000000.0;
    durations[0] = duration;
  }, durations);
  // Bob
  threads.emplace_back([&alice, &bob, &pond, &occupant](double durations[]){
    struct timespec cbegin, cend;
    double duration;
    clock_gettime(CLOCK_MONOTONIC, &cbegin);
    for(int i = 0; i < NFEEDS; i++) {
      bob.lock(alice);
      occupant = BOB;
      pond++;
      assert(occupant == BOB);
      bob.unlock();
    }
    clock_gettime(CLOCK_MONOTONIC, &cend);
    duration = (cend.tv_sec - cbegin.tv_sec) + (cend.tv_nsec - cbegin.tv_nsec) / 1000000000.0;
    durations[1] = duration;
  }, durations);

  for(auto& thread: threads) {
    thread.join();
  }
  assert(pond == 2 * NFEEDS);
  std::cout << "Access: " << pond << std::endl;
  std::cout << "Alice's duration: " << durations[0] << "s" << std::endl;
  std::cout << "Bob's duration  : " << durations[1] << "s" << std::endl;

  return 0;
}