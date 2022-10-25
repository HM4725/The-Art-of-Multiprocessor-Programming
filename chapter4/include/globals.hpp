#include "base/snapshot.hpp"

namespace thread {
  extern thread_local int thread_id;
}

namespace shared {
  extern Snapshot<int> *snapshot;
  extern bool finish;
}
