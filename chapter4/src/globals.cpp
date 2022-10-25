#include <queue>
#include "base/snapshot.hpp"

namespace thread {
  thread_local int thread_id;
}

namespace shared {
  Snapshot<int>* snapshot;
  bool finish;
}
