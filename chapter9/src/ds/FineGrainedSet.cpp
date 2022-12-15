#include "FineGrainedSet.h"
#include <mutex>

FineGrainedSet::Node::Node() : key(0), next(nullptr) {}
FineGrainedSet::Node::Node(int64 key) : key(key), next(nullptr) {}
FineGrainedSet::Node::Node(int64 key, Node *next) : key(key), next(next) {}
FineGrainedSet::Node::Node(const Node &n) : key(n.key), next(n.next) {}

FineGrainedSet::FineGrainedSet() {
  // Sentinel Nodes
  tail = new Node(INT64_MAX);
  head = new Node(INT64_MIN, tail);
}

FineGrainedSet::~FineGrainedSet() {
  Node *curr, *freed;

  curr = head;
  while (curr != nullptr) {
    freed = curr;
    curr = curr->next;
    delete freed;
  }
}

bool FineGrainedSet::add(int64 key) {
  Node *n;
  Node *pred, *curr;

  // Traverse and find window
  pred = head;
  pred->m.lock();
  curr = head->next;
  curr->m.lock();
  while (curr->key < key) {
    pred->m.unlock();
    pred = curr;
    curr = curr->next;
    curr->m.lock();
  }

  // Exception: Already exist
  if (curr->key == key) {
    pred->m.unlock();
    curr->m.unlock();
    return false;
  }

  // Add Main Routine
  n = new Node(key, curr);
  pred->next = n;
  pred->m.unlock();
  curr->m.unlock();

  return true;
}

bool FineGrainedSet::remove(int64 key) {
  Node *pred, *curr;

  // Traverse and find window
  pred = head;
  pred->m.lock();
  curr = head->next;
  curr->m.lock();
  while (curr->key < key) {
    pred->m.unlock();
    pred = curr;
    curr = curr->next;
    curr->m.lock();
  }

  // Exception: Not exist
  if (curr->key != key) {
    pred->m.unlock();
    curr->m.unlock();
    return false;
  }

  // Remove Main Routine
  pred->next = curr->next;
  pred->m.unlock();
  curr->m.unlock();
  delete curr;

  return true;
}

bool FineGrainedSet::contains(int64 key) {
  Node *pred, *curr;
  bool res;

  // Contains
  pred = head;
  pred->m.lock();
  curr = head->next;
  curr->m.lock();
  while (curr->key < key) {
    pred->m.unlock();
    pred = curr;
    curr = curr->next;
    curr->m.lock();
  }
  res = curr->key == key;

  pred->m.unlock();
  curr->m.unlock();

  return res;
}

size_t FineGrainedSet::size() {
  size_t sz = 0;
  for (Node *curr = head->next; curr != tail; curr = curr->next) {
    sz += 1;
  }
  return sz;
}