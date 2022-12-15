#include "CoarseGrainedSet.h"
#include <mutex>

CoarseGrainedSet::Node::Node() : key(0), next(nullptr) {}
CoarseGrainedSet::Node::Node(int64 key) : key(key), next(nullptr) {}
CoarseGrainedSet::Node::Node(int64 key, Node *next) : key(key), next(next) {}
CoarseGrainedSet::Node::Node(const Node &n) : key(n.key), next(n.next) {}

CoarseGrainedSet::CoarseGrainedSet() {
  // Sentinel Nodes
  tail = new Node(INT64_MAX);
  head = new Node(INT64_MIN, tail);
}

CoarseGrainedSet::~CoarseGrainedSet() {
  Node *curr, *freed;

  curr = head;
  while (curr != nullptr) {
    freed = curr;
    curr = curr->next;
    delete freed;
  }
}

bool CoarseGrainedSet::add(int64 key) {
  Node *n;
  Node *pred, *curr;

  // Traverse and find window
  m.lock();
  pred = head;
  curr = head->next;
  while (curr->key < key) {
    pred = curr;
    curr = curr->next;
  }

  // Exception: Already exist
  if (curr->key == key) {
    m.unlock();
    return false;
  }

  // Add Main Routine
  n = new Node(key, curr);
  pred->next = n;
  m.unlock();

  return true;
}

bool CoarseGrainedSet::remove(int64 key) {
  Node *pred, *curr;

  // Traverse and find window
  m.lock();
  pred = head;
  curr = head->next;
  while (curr->key < key) {
    pred = curr;
    curr = curr->next;
  }

  // Exception: Not exist
  if (curr->key != key) {
    m.unlock();
    return false;
  }

  // Remove Main Routine
  pred->next = curr->next;
  m.unlock();
  delete curr;

  return true;
}

bool CoarseGrainedSet::contains(int64 key) {
  Node *curr;
  bool res;

  m.lock();

  // Contains
  curr = head;
  while (curr->key < key) {
    curr = curr->next;
  }
  res = curr->key == key;

  m.unlock();

  return res;
}

size_t CoarseGrainedSet::size() {
  size_t sz = 0;
  for (Node *curr = head->next; curr != tail; curr = curr->next) {
    sz += 1;
  }
  return sz;
}
