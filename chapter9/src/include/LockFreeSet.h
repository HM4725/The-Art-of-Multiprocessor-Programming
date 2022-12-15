#ifndef __LOCK_FREE_SET_H__
#define __LOCK_FREE_SET_H__

#include "setInterface.h"
#include <atomic>

class LockFreeSet : public SetInterface {
private:
  struct Node {
    int64 key;
    std::atomic<Node *> next;

    Node();
    Node(int64 key);
    Node(int64 key, Node *next);
    Node(const Node &n);
  };

  struct Window {
    Node *pred;
    Node *curr;
    Window() = delete;
    Window(Node *pred, Node *curr);
    Window(const Window &w);
  };

  Node *head;
  Node *tail;

  Window find_window(int64 key);

public:
  LockFreeSet();
  ~LockFreeSet();
  bool add(int64 key);
  bool remove(int64 key);
  bool contains(int64 key);
  size_t size();
};

#endif // __LOCK_FREE_SET_H__