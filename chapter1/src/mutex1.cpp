/* Page 7, Figure 1.4
 *
 * 1. mutual-exclusion: o
 * 2. dead-lock freedom: x
 */

#include <iostream>
#include <vector>
#include <thread>

#define NFEEDS 10000

class BobProtocol;

class AliceProtocol {
  private:
    bool flag;
    friend class BobProtocol;
  public:
    void lock(BobProtocol& bob);
    void unlock();
    AliceProtocol() : flag(0) {}
};

class BobProtocol {
  private:
    bool flag;
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
  while(alice.flag);
}

void BobProtocol::unlock() {
  flag = 0;
}

// == MAIN ============================
int main() {
  std::vector<std::thread> threads;
  AliceProtocol alice;
  BobProtocol bob;

  // Shared Object
  int pond = 0;

  // Alice
  threads.emplace_back([&alice, &bob, &pond](){
    for(int i = 0; i < NFEEDS; i++) {
      alice.lock(bob);
      pond++;
      alice.unlock();
    }
  });
  // Bob
  threads.emplace_back([&alice, &bob, &pond](){
    for(int i = 0; i < NFEEDS; i++) {
      bob.lock(alice);
      pond++;
      bob.unlock();
    }
  });

  for(auto& thread: threads) {
    thread.join();
  }
  std::cout << "Used: " << pond << std::endl;

  return 0;
}