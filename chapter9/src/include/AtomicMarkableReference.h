#ifndef __ATOMIC_MARKABLE_REFERENCE_H__
#define __ATOMIC_MARKABLE_REFERENCE_H__

#include "types.h"

namespace amr {

template <typename T> inline bool is_marked(T p) {
  return reinterpret_cast<uintptr_t>(p) & 0x8000000000000000;
}

template <typename T> inline T marked(T p) {
  return reinterpret_cast<T>(reinterpret_cast<uintptr_t>(p) |
                             0x8000000000000000);
}
template <typename T> inline T unmarked(T p) {
  return reinterpret_cast<T>(reinterpret_cast<uintptr_t>(p) &
                             0x7FFFFFFFFFFFFFFF);
}
template <typename T> inline T reference(T p) {
#ifdef __APPLE_CC__
  return p;
#else
  return unmarked(p);
#endif
}

} // namespace amr

#endif // __ATOMIC_MARKABLE_REFERENCE_H__