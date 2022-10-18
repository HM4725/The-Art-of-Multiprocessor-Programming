#include <pthread.h>
#include <stdio.h>

#define DEFAULT -1
#define TOTAL_LINES 61632

// Shared
volatile int32_t proposed[2];
int flag = 0;
// Thread locals
int32_t inputs[2];

void propose(int32_t value, int64_t id) {
  proposed[id] = value;
}

int32_t decide(int32_t value, int64_t id) {
  propose(value, id);
  if(flag == 0) {
    flag = 1;
    return value;
  } else {
    return proposed[1-id];
  }
}

void* worker(void* args) {
  int64_t id = (int64_t)args;
  int32_t value = inputs[id];
  int32_t result = decide(value, id);
  return (void*)(int64_t)result;
}

int main() {
  pthread_t threads[2];
  int32_t decided[2];
  int nfails = 0;
  int counter = 0;
  while(1) {
    // init
    inputs[0] = DEFAULT;
    inputs[1] = DEFAULT;
    proposed[0] = DEFAULT;
    proposed[1] = DEFAULT;
    flag = 0;

    // input values
    scanf("%d", &inputs[0]);
    if(inputs[0] < 0) {
      break;
    }
    scanf("%d", &inputs[1]);

    // thread
    pthread_create(&threads[0], NULL, worker, (void*)0);
    pthread_create(&threads[1], NULL, worker, (void*)1);

    pthread_join(threads[0], (void**)&decided[0]);
    pthread_join(threads[1], (void**)&decided[1]);

    if(decided[0] != decided[1]) {
      nfails += 1;
    }
    counter += 1;
    printf("\x1B[2J\x1B[H");
    printf("[%d / %d]\n", counter, TOTAL_LINES);
    printf("Fails: %d\n", nfails);
  }
  return 0;
}