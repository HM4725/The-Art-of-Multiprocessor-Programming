#pragma once

#include <cassert>
#include <vector>
#include <queue>
#include "base/snapshot.hpp"
#include "snap_value.hpp"
#include "array.hpp"
#include "globals.hpp"


template <typename T>
class WaitFreeSnapshot : public Snapshot<T> {

  public:
    const int N;
    ObjectArray<stamp::SnapValue<T>> *table;
    std::vector<std::queue<stamp::SnapValue<T>*>*> __gc;
    typename std::queue<stamp::SnapValue<T>*>::size_type __gc_capacity;

    ObjectArray<stamp::SnapValue<T>>* _collect() {
      return new ObjectArray<stamp::SnapValue<T>>(*table);
    }

    Array<T>* _scan() {
      ObjectArray<stamp::SnapValue<T>> *oldCopy, *newCopy;
      Array<T> *result;
      bool* moved = new bool[N]();
      oldCopy = _collect();
      collect:
      while(true) {
        newCopy = _collect();
        for(int j = 0; j < N; j++) {
          if(oldCopy->ref(j) != newCopy->ref(j)) {
            if(moved[j]) {
              result = newCopy->ref(j).stealSnap();
              delete oldCopy;
              delete newCopy;
              delete[] moved;
              return result;
            } else {
              moved[j] = true;
              delete oldCopy;
              oldCopy = newCopy;
              goto collect;
            }
          }
        }
        result = new Array<T>(N);
        for(int i = 0; i < N; i++) {
          (*result)[i] = newCopy->ref(i).getValue();
        }
        delete oldCopy;
        delete newCopy;
        delete[] moved;
        return result;
      }
    }

  public:
    WaitFreeSnapshot() = delete;
    WaitFreeSnapshot(int N) : N(N), __gc_capacity(5000000) {
      table = new ObjectArray<stamp::SnapValue<T>>(N);
      __gc.reserve(N);
      for(int i = 0; i < N; i++) {
        __gc.emplace_back(new std::queue<stamp::SnapValue<T>*>());
      }
    }
    ~WaitFreeSnapshot() {
      delete table;
      for(auto& q: __gc) {
        while(!q->empty()) {
          delete q->front();
          q->pop();
        }
        delete q;
      }
    }
    void update(T v) {
      int me = thread::thread_id;
      std::queue<stamp::SnapValue<T>*>* gc = __gc[me];
      Array<T> *snap = _scan();
      stamp::SnapValue<T>* oldValue = table->get(me);
      stamp::SnapValue<T>* newValue = \
        new stamp::SnapValue<T>(oldValue->getLabel() + 1, v, snap);
      table->set(me, newValue);
      gc->push(oldValue);
      if(gc->size() > __gc_capacity) {
        delete gc->front();
        gc->pop();
      }
      delete snap;
    }
    T* scan() {
      Array<T> *snap = _scan();
      T* result = new T[N];
      for(int i = 0; i < N; i++) {
        result[i] = (*snap)[i];
      }
      delete snap;
      return result;
    }
};