#include "OptimisticSet.h"
#include <mutex>

OptimisticSet::Node::Node() : key(0), next(nullptr) {}
OptimisticSet::Node::Node(int64 key) : key(key), next(nullptr) {}
OptimisticSet::Node::Node(int64 key, Node *next) : key(key), next(next) {}
OptimisticSet::Node::Node(const Node &n) : key(n.key), next(n.next) {}

bool OptimisticSet::is_valid(Node* pred, Node* curr) {
  Node *itr = head;
  while(itr->key <= pred->key) {
    if(itr == pred) {
      return itr->next == curr;
    }
    itr = itr->next;
  }
  return false;
}

OptimisticSet::OptimisticSet() {
  // Sentinel Nodes
  tail = new Node(INT64_MAX);
  head = new Node(INT64_MIN, tail);
}

OptimisticSet::~OptimisticSet() {
  Node *curr, *freed;

  curr = head;
  while (curr != nullptr) {
    freed = curr;
    curr = curr->next;
    delete freed;
  }
}

bool OptimisticSet::add(int64 key) {
  Node *n;
  Node *pred, *curr;

  while(true) {
    // Traverse and find window
    pred = head;
    curr = head->next;
    while(curr->key < key) {
      pred = curr;
      curr = curr->next;
    }
    pred->m.lock();
    curr->m.lock();

    // Validation: Traverse again
    if(!is_valid(pred, curr)) {
      pred->m.unlock();
      curr->m.unlock();
      continue;
    }

    // Exception: Already exist
    if(curr->key == key) {
      pred->m.unlock();
      curr->m.unlock();
      return false;
    }
    
    // Add Main Routine
    n = new Node(key, curr);
    std::atomic_thread_fence(std::memory_order_release);
    pred->next = n;
    pred->m.unlock();
    curr->m.unlock();
    break;
  }
  return true;
}

bool OptimisticSet::remove(int64 key) {
  Node *pred, *curr;

  while(true) {
    // Traverse and find window
    pred = head;
    curr = head->next;
    while(curr->key < key) {
      pred = curr;
      curr = curr->next;
    }
    pred->m.lock();
    curr->m.lock();

    // Validation: Traverse again
    if(!is_valid(pred, curr)) {
      pred->m.unlock();
      curr->m.unlock();
      continue;
    }

    // Exception: No exist
    if(curr->key != key) {
      pred->m.unlock();
      curr->m.unlock();
      return false;
    }
    
    // Remove Main Routine
    pred->next = curr->next;
    pred->m.unlock();
    curr->m.unlock();
    // TODO: Need GC
    // delete curr;
    break;
  }
  return true;
}

bool OptimisticSet::contains(int64 key) {
  Node *pred, *curr;
  bool res = false;

  while(true) {
    // Traverse and find window
    pred = head;
    curr = head->next;
    while(curr->key < key) {
      pred = curr;
      curr = curr->next;
    }
    pred->m.lock();
    curr->m.lock();

    // Validation: Traverse again
    if(!is_valid(pred, curr)) {
      pred->m.unlock();
      curr->m.unlock();
      continue;
    }

    // Contains Main Routine
    res = curr->key == key;
    pred->m.unlock();
    curr->m.unlock();
    break;
  }
  return res;
}

size_t OptimisticSet::size() {
  size_t sz = 0;
  for(Node *curr = head->next; curr != tail; curr = curr->next) {
    sz += 1;
  }
  return sz;
}
