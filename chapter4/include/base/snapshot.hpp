#pragma once

#include <memory>

template <typename T>
class Snapshot {
  public:
    virtual ~Snapshot() = default;
    virtual T* scan() = 0;
    virtual void update(T v) = 0;
};