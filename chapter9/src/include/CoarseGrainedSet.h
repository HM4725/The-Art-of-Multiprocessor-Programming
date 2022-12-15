#ifndef __COARSE_GRAINED_SET_H__
#define __COARSE_GRAINED_SET_H__

#include "setInterface.h"
#include <mutex>

class CoarseGrainedSet : public SetInterface {
private:
  struct Node {
    int64 key;
    Node *next;

    Node();
    Node(int64 key);
    Node(int64 key, Node *next);
    Node(const Node &n);
  };

  std::mutex m;
  Node *head;
  Node *tail;

public:
  CoarseGrainedSet();
  ~CoarseGrainedSet();
  bool add(int64 key);
  bool remove(int64 key);
  bool contains(int64 key);
  size_t size();
};

#endif // __COARSE_GRAINED_SET_H__