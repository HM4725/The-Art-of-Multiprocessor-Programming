#include "LockFreeSet.h"
#include "AtomicMarkableReference.h"
#include <cassert>

LockFreeSet::Node::Node() : key(0), next(nullptr) {}
LockFreeSet::Node::Node(int64 key) : key(key), next(nullptr) {}
LockFreeSet::Node::Node(int64 key, Node *next) : key(key), next(next) {}
LockFreeSet::Node::Node(const Node &n) : key(n.key), next(n.next.load()) {}

LockFreeSet::Window::Window(LockFreeSet::Node *pred, LockFreeSet::Node *curr)
    : pred(pred), curr(curr) {}
LockFreeSet::Window::Window(const Window &w) : pred(w.pred), curr(w.curr) {}

LockFreeSet::Window LockFreeSet::find_window(int64 key) {
  Node *pred, *curr;

retry:
  for (pred = head, curr = amr::reference<Node *>(head->next);;
       pred = curr, curr = amr::reference<Node *>(curr->next)) {
    for (; amr::is_marked<Node *>(curr->next);
         curr = amr::reference<Node *>(curr->next)) {
      Node *unmarked_curr = amr::unmarked<Node *>(curr);
      Node *unmarked_succ = amr::unmarked<Node *>(curr->next);
      if (!pred->next.compare_exchange_weak(unmarked_curr, unmarked_succ)) {
        goto retry;
      }
    }
    if (curr->key >= key) {
      return Window(pred, curr);
    }
  }
}

LockFreeSet::LockFreeSet() {
  // Sentinel Nodes
  tail = new Node(INT64_MAX);
  head = new Node(INT64_MIN, tail);
}

LockFreeSet::~LockFreeSet() {
  Node *curr, *freed;

  curr = head;
  while (curr != nullptr) {
    freed = curr;
    curr = curr->next.load();
    delete freed;
  }
}

bool LockFreeSet::add(int64 key) {
  Node *n;
  Node *pred, *curr;

  while (true) {
    // Traverse and find window
    Window w = find_window(key);
    pred = w.pred;
    curr = w.curr;

    // Exception: Already exist
    if (curr->key == key) {
      return false;
    }

    // Add Main Routine
    Node *succ = amr::unmarked<Node *>(curr);
    n = new Node(key, succ);
    if (pred->next.compare_exchange_weak(succ, n)) {
      break;
    }
    delete n;
  }
  return true;
}

bool LockFreeSet::remove(int64 key) {
  Node *pred, *curr;

  while (true) {
    // Traverse and find window
    Window w = find_window(key);
    pred = w.pred;
    curr = w.curr;

    // Exception: No exist
    if (curr->key != key) {
      return false;
    }

    // Remove Main Routine
    Node *succ = curr->next.load();
    Node *marked_succ = amr::marked<Node *>(succ);
    Node *unmarked_succ = amr::unmarked<Node *>(succ);
    if (curr->next.compare_exchange_weak(unmarked_succ, marked_succ)) {
      Node *unmarked_curr = amr::unmarked<Node *>(curr);
      pred->next.compare_exchange_strong(unmarked_curr, succ);
      // TODO: Need GC
      // delete unmarked_curr;
      break;
    } else {
      return false;
    }
  }
  return true;
}

bool LockFreeSet::contains(int64 key) {
  Node *curr = head;
  while (curr->key < key) {
    curr = amr::reference<Node *>(curr->next);
  }
  return curr->key == key && !amr::is_marked<Node *>(curr->next);
}

size_t LockFreeSet::size() {
  Node *curr = head;
  size_t sz = 0;
  for (curr = head->next; curr != tail;
       curr = amr::reference<Node *>(curr->next)) {
    if (!amr::is_marked<Node *>(curr)) {
      sz += 1;
    }
  }
  return sz;
}
