#pragma once

#include <stdexcept>

template <class T>
class ObjectArray {
  private:
    std::vector<T*> data;
    const int N;
  public:
    ObjectArray() = delete;
    ObjectArray(int N) : N(N) {
      data.reserve(N);
      for(int i = 0; i < N; i++) {
        data.emplace_back(new T());
      }
    }
    ObjectArray(const ObjectArray<T> &arr) : N(arr.N) {
      data.reserve(N);
      for(int i = 0; i < N; i++) {
        data.emplace_back(new T(arr.data[i]));
      }
    }
    ~ObjectArray() {
      for(auto& v: data) {
        delete v;
      }
    }
    const int size() {
      return N;
    }
    T* get(int idx) {
      return data[idx];
    }
    T& ref(int idx) {
      return std::ref(*data[idx]);
    }
    void set(int idx, T* v) {
      data[idx] = v;
    }
};

// Primitive Type
template <typename T>
class Array {
  private:
    const int N;
    std::vector<T> data;
  public:
    Array() = delete;
    Array(int N) : N(N) {
      data.assign(N, 0);
    }
    Array(Array<T>* parr) : N(parr->N) {
      data.assign(parr->data.begin(), parr->data.end());
    }
    Array(const Array<T>& arr) : N(arr.N) {
      data.assign(arr.data.begin(), arr.data.end());
    }
    T& operator[](int idx) {
      if(idx < 0 || idx >= N)
        throw std::out_of_range("Out of Range!");
      return data[idx];
    }
};