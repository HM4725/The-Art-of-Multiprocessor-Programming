#ifndef __LAZY_SYNC_SET_H__
#define __LAZY_SYNC_SET_H__

#include "setInterface.h"
#include <mutex>

class LazySyncSet : public SetInterface {
private:
  struct Node {
    int64 key;
    Node *next;
    std::mutex m;
    bool marked;

    Node();
    Node(int64 key);
    Node(int64 key, Node *next);
    Node(const Node &n);
  };

  Node *head;
  Node *tail;

  bool is_valid(Node *pred, Node *curr);

public:
  LazySyncSet();
  ~LazySyncSet();
  bool add(int64 key);
  bool remove(int64 key);
  bool contains(int64 key);
  size_t size();
};

#endif // __LAZY_SYNC_SET_H__