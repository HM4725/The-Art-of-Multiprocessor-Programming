#pragma once

#include <vector>

template <typename T> class ConsensusN {
private:
  std::vector<T *> proposed;
  const size_t N;
  std::atomic<int> order;
  static constexpr int PENDING = -1;

  void propose(int id, T *v) { proposed[id] = v; }

public:
  ConsensusN(size_t N) : N(N), order(PENDING) { proposed.resize(N, nullptr); }
  T *decide(int id, T *v) {
    int yet = PENDING;
    propose(id, v);
    order.compare_exchange_strong(yet, id);
    return proposed[order.load()];
  }
};