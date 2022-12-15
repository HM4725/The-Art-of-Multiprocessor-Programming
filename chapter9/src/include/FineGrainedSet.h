#ifndef __FINE_GRAINED_SET_H__
#define __FINE_GRAINED_SET_H__

#include "setInterface.h"
#include <mutex>

class FineGrainedSet : public SetInterface {
private:
  struct Node {
    int64 key;
    Node *next;
    std::mutex m;

    Node();
    Node(int64 key);
    Node(int64 key, Node *next);
    Node(const Node &n);
  };

  Node *head;
  Node *tail;

public:
  FineGrainedSet();
  ~FineGrainedSet();
  bool add(int64 key);
  bool remove(int64 key);
  bool contains(int64 key);
  size_t size();
};

#endif // __FINE_GRAINED_SET_H__