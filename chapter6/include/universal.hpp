#pragma once

#include "consensus.hpp"
#include "method.hpp"
#include "sequential.hpp"
#include <algorithm>
#include <functional>
#include <vector>

template <typename T, class SeqDerived> class LFUniversal;
template <typename T, class SeqDerived> class WFUniversal;
template <typename T, class SeqDerived> class HMUniversal;

// Comparable
template <typename T, class SeqDerived> class Node {
private:
  Invoc<T> invoc;
  ConsensusN<Node> *decideNext;
  Node *next;
  int seq;
  const size_t N;
  friend class LFUniversal<T, SeqDerived>;
  friend class WFUniversal<T, SeqDerived>;
  friend class HMUniversal<T, SeqDerived>;

public:
  Node() = delete;
  Node(size_t n)
      : decideNext(new ConsensusN<Node>(n)), next(nullptr), seq(0), N(n) {
  } // bogus
  Node(const Invoc<T> &invoc, size_t n)
      : invoc(invoc), decideNext(new ConsensusN<Node>(n)), next(nullptr),
        seq(0), N(n) {}
  ~Node() { delete decideNext; }
};

template <typename T, class SeqDerived> class LFUniversal {
private:
  std::vector<Node<T, SeqDerived> *> heads;
  SeqObject<T, SeqDerived> *obj;
  Node<T, SeqDerived> *tail;
  const size_t N;
  Node<T, SeqDerived> *maxNode() {
    Node<T, SeqDerived> *max = heads[0];
    for (int i = 1; i < N; i++) {
      if (max->seq < heads[i]->seq) {
        max = heads[i];
      }
    }
    return max;
  }

public:
  LFUniversal(size_t n) : tail(new Node<T, SeqDerived>(n)), N(n) {
    tail->seq = 1;
    heads.resize(n, tail);
    obj = new SeqDerived();
  }
  ~LFUniversal() { delete obj; }
  Response<T> apply(int id, const Invoc<T> &invoc) {
    Node<T, SeqDerived> *prefer = new Node<T, SeqDerived>(invoc, N);
    while (prefer->seq == 0) {
      Node<T, SeqDerived> *before = maxNode();
      Node<T, SeqDerived> *after = before->decideNext->decide(id, prefer);
      before->next = after;
      after->seq = before->seq + 1;
      heads[id] = after;
    }
    // Compute my response
    SeqObject<T, SeqDerived> *myObject = obj->clone();
    Node<T, SeqDerived> *current = tail->next;
    while (current != prefer) {
      myObject->apply(current->invoc);
      current = current->next;
    }
    Response<T> res = myObject->apply(current->invoc);
    delete myObject;
    return res;
  }
};

template <typename T, class SeqDerived> class WFUniversal {
private:
  std::vector<Node<T, SeqDerived> *> announces;
  std::vector<Node<T, SeqDerived> *> heads;
  SeqObject<T, SeqDerived> *obj;
  Node<T, SeqDerived> *tail;
  const size_t N;
  Node<T, SeqDerived> *maxNode() {
    Node<T, SeqDerived> *max = heads[0];
    for (int i = 1; i < N; i++) {
      if (max->seq < heads[i]->seq) {
        max = heads[i];
      }
    }
    return max;
  }

public:
  WFUniversal(size_t n) : tail(new Node<T, SeqDerived>(n)), N(n) {
    tail->seq = 1;
    announces.resize(n, tail);
    heads.resize(n, tail);
    obj = new SeqDerived();
  }
  ~WFUniversal() { delete obj; }
  Response<T> apply(int id, const Invoc<T> &invoc) {
    Node<T, SeqDerived> *prefer;
    announces[id] = new Node<T, SeqDerived>(invoc, N);
    heads[id] = maxNode();
    while (announces[id]->seq == 0) {
      Node<T, SeqDerived> *before = heads[id];
      Node<T, SeqDerived> *help = announces[(before->seq + 1) % N];
      if (help->seq == 0) {
        prefer = help;
      } else {
        prefer = announces[id];
      }
      Node<T, SeqDerived> *after = before->decideNext->decide(id, prefer);
      before->next = after;
      after->seq = before->seq + 1;
      heads[id] = after;
    }
    // Compute my response
    SeqObject<T, SeqDerived> *myObject = obj->clone();
    Node<T, SeqDerived> *current = tail->next;
    while (current != announces[id]) {
      myObject->apply(current->invoc);
      current = current->next;
    }
    Response<T> res = myObject->apply(current->invoc);
    delete myObject;
    return res;
  }
};

template <typename T, class SeqDerived> class HMUniversal {
private:
  SeqObject<T, SeqDerived> *obj;
  const Node<T, SeqDerived> *tail;
  std::atomic<Node<T, SeqDerived> *> head;
  const size_t N;

public:
  HMUniversal(size_t n)
      : obj(new SeqDerived()), tail(new Node<T, SeqDerived>(n)), head(tail),
        N(n) {}
  ~HMUniversal() { delete obj; }
  Response<T> apply(int id, const Invoc<T> &invoc) {
    Node<T, SeqDerived> *prefer = new Node<T, SeqDerived>(invoc, N);
    for(Node<T, SeqDerived> *before = head.load(); ;before = before->next) {
      if(__sync_bool_compare_and_exchange(&before->next, nullptr, prefer)) {
        prefer->seq = before->seq + 1;
        if(prefer->seq > head->seq) {
          head.store(prefer);
        }
        break;
      }
    }

    // Compute my response
    SeqObject<T, SeqDerived> *myObject = obj->clone();
    Node<T, SeqDerived> *current = tail->next;
    while (current != prefer) {
      myObject->apply(current->invoc);
      current = current->next;
    }
    Response<T> res = myObject->apply(current->invoc);
    delete myObject;
    return res;
  }
};