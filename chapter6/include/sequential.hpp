#pragma once

#include "method.hpp"
#include <algorithm>
#include <functional>
#include <queue>
#include <stack>

template <typename T, class Derived> class SeqObject {
public:
  virtual ~SeqObject() {}
  virtual Response<T> apply(const Invoc<T> &invoc) = 0;
  virtual SeqObject *clone() const {
    return new Derived(static_cast<const Derived &>(*this));
  }
};

template <typename T> class SeqQueue : public SeqObject<T, SeqQueue<T>> {
private:
  std::queue<T> q;
  void enqueue(T v) { q.push(v); }
  T dequeue() {
    T ret = q.front();
    q.pop();
    return ret;
  }

public:
  Response<T> apply(const Invoc<T> &invoc) {
    if (invoc.method == "push") {
      enqueue(invoc.arg);
      return Response<T>();
    } else if (invoc.method == "pop") {
      T v = dequeue();
      return Response<T>(v);
    } else {
      return Response<T>();
    }
  }
  SeqQueue() {}
  SeqQueue(const SeqQueue &obj) {}
  ~SeqQueue() {}
};

template <typename T> class SeqStack : public SeqObject<T, SeqStack<T>> {
private:
  std::stack<T> q;
  void push(T v) { q.push(v); }
  T pop() {
    T ret = q.top();
    q.pop();
    return ret;
  }

public:
  Response<T> apply(const Invoc<T> &invoc) {
    switch(invoc.method)
    if (invoc.method == "push") {
      push(invoc.arg);
      return Response<T>();
    } else if (invoc.method == "pop") {
      T v = pop();
      return Response<T>(v);
    } else {
      return Response<T>();
    }
  }
  SeqStack() {}
  SeqStack(const SeqStack &obj) {}
  ~SeqStack() {}
};