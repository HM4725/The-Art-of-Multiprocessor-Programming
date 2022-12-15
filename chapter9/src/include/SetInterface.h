#ifndef __SET_INTERFACE_H__
#define __SET_INTERFACE_H__

#include "types.h"

class SetInterface {
public:
  /* Concurrent Methods
   *   These method are called concurrently.
   */
  virtual bool add(int64) = 0;
  virtual bool remove(int64) = 0;
  virtual bool contains(int64) = 0;

  /* Validation Check Method
   *   This method has to be called alone.
   */
  virtual size_t size() = 0;
};

#endif // __SET_INTERFACE_H__