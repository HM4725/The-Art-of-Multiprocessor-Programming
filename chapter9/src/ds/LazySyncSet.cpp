#include "LazySyncSet.h"
#include <mutex>

LazySyncSet::Node::Node() : key(0), next(nullptr), marked(false) {}
LazySyncSet::Node::Node(int64 key) : key(key), next(nullptr), marked(false) {}
LazySyncSet::Node::Node(int64 key, Node *next)
    : key(key), next(next), marked(false) {}
LazySyncSet::Node::Node(const Node &n)
    : key(n.key), next(n.next), marked(false) {}

bool LazySyncSet::is_valid(Node *pred, Node *curr) {
  return !pred->marked && !curr->marked && pred->next == curr;
}

LazySyncSet::LazySyncSet() {
  // Sentinel Nodes
  tail = new Node(INT64_MAX);
  head = new Node(INT64_MIN, tail);
}

LazySyncSet::~LazySyncSet() {
  Node *curr, *freed;

  curr = head;
  while (curr != nullptr) {
    freed = curr;
    curr = curr->next;
    delete freed;
  }
}

bool LazySyncSet::add(int64 key) {
  Node *n;
  Node *pred, *curr;

  while (true) {
    // Traverse and find window
    pred = head;
    curr = head->next;
    while (curr->key < key) {
      pred = curr;
      curr = curr->next;
    }
    pred->m.lock();
    curr->m.lock();

    // Validation: Traverse again
    if (!is_valid(pred, curr)) {
      pred->m.unlock();
      curr->m.unlock();
      continue;
    }

    // Exception: Already exist
    if (curr->key == key) {
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

bool LazySyncSet::remove(int64 key) {
  Node *pred, *curr;

  while (true) {
    // Traverse and find window
    pred = head;
    curr = head->next;
    while (curr->key < key) {
      pred = curr;
      curr = curr->next;
    }
    pred->m.lock();
    curr->m.lock();

    // Validation: Traverse again
    if (!is_valid(pred, curr)) {
      pred->m.unlock();
      curr->m.unlock();
      continue;
    }

    // Exception: No exist
    if (curr->key != key) {
      pred->m.unlock();
      curr->m.unlock();
      return false;
    }

    // Remove Main Routine
    curr->marked = true;
    pred->next = curr->next;
    pred->m.unlock();
    curr->m.unlock();
    // TODO: Need GC
    // delete curr;
    break;
  }
  return true;
}

bool LazySyncSet::contains(int64 key) {
  Node *curr;
  for(curr = head; curr->key < key; curr = curr->next);
  return curr->key == key && !curr->marked;
}

size_t LazySyncSet::size() {
  size_t sz = 0;
  for (Node *curr = head->next; curr != tail; curr = curr->next) {
    sz += 1;
  }
  return sz;
}
