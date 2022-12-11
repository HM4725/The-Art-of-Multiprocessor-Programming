#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_NUM_THREAD 8
#define MAX_NUM_WORK 100000

#define ISMARKED(p) ((unsigned long long)p & 0x8000000000000000)
#define MARKED(p) ((unsigned long long)p | 0x8000000000000000)
#define UNMARKED(p) ((unsigned long long)p & 0x7FFFFFFFFFFFFFFF)
#ifdef __APPLE_CC__
#define REFERENCE(p) p
#else
#define REFERENCE(p) UNMARKED(p)
#endif

// list node structure
struct Node {
  int key;
  Node *next;
  Node() = delete;
  Node(int key) : key(key), next(nullptr) {}
};

// linked list
Node *head;
Node *tail;

// Window
struct Window {
  Node *prev;
  Node *curr;
  Window(Node *prev, Node *curr) : prev(prev), curr(curr) {}
  Window(const Window &w) : prev(w.prev), curr(w.curr) {}
};

// workload data
struct Work {
  char type;
  int value;
};

Work thread_work[MAX_NUM_THREAD][MAX_NUM_WORK];
int num_work;

Window find(Node *head, int key) {
  Node *prev, *curr;

retry:
  for (prev = head, curr = (Node *)REFERENCE(head->next);;
       prev = curr, curr = (Node *)REFERENCE(curr->next)) {
    for (; ISMARKED(curr->next); curr = (Node *)REFERENCE(curr->next)) {
      if (!__sync_bool_compare_and_swap(&prev->next, (Node *)UNMARKED(curr),
                                        (Node *)UNMARKED(curr->next)))
        goto retry;
    }
    if (curr->key >= key)
      return Window(prev, curr);
  }
}

// initialize linked list
void list_init(Node **head, Node **tail) {
  *head = new Node(INT_MIN);
  *tail = new Node(INT_MAX);

  (*head)->next = *tail;
  (*tail)->next = nullptr;
}

// add key into the list
bool list_add(int key) {
  Node *node = new Node(key);
  Node *prev, *curr;

  while (1) {
    Window w = find(head, key);
    prev = w.prev;
    curr = w.curr;
    if (curr->key == key) {
      return false;
    }

    node->next = (Node *)UNMARKED(curr);
    if (__sync_bool_compare_and_swap(&prev->next, (Node *)UNMARKED(curr),
                                     node)) {
      break;
    }
  }

  return true;
}

// remove key from the list
bool list_remove(int key) {
  Node *prev, *curr;

  while (1) {
    Window w = find(head, key);
    prev = w.prev;
    curr = w.curr;
    if (curr->key != key) {
      return false;
    }

    Node *succ = curr->next;
    if (__sync_bool_compare_and_swap(&curr->next, (Node *)UNMARKED(succ),
                                     (Node *)MARKED(succ))) {
      __sync_bool_compare_and_swap(&prev->next, (Node *)UNMARKED(curr), succ);
      break;
    }
  }

  return true;
}

// check whether a key is in the list
bool list_contains(int key) {
  Node *curr;
  for (curr = head; curr->key < key; curr = (Node *)REFERENCE(curr->next))
    ;

  return curr->key == key && !ISMARKED(curr->next);
}

// do workloads for each threads
void *ThreadFunc(void *args) {
  long tid = (long)args;

  for (int i = 0; i < num_work; i++) {
    switch (thread_work[tid][i].type) {
    case 'A':
      list_add(thread_work[tid][i].value);
      break;
    case 'R':
      list_remove(thread_work[tid][i].value);
      break;
    case 'C':
      list_contains(thread_work[tid][i].value);
      break;
    default:
      break;
    }
  }

  return NULL;
}

int main(void) {
  int num_thread;

  // input workloads
  FILE *fp = fopen("workload.txt", "r");

  fscanf(fp, "%d %d", &num_thread, &num_work);

  int thread_num;
  char work_type;
  int value;
  for (int i = 0; i < num_thread; i++) {
    fscanf(fp, "%d ", &thread_num);

    for (int j = 0; j < num_work; j++) {
      fscanf(fp, "%c %d ", &work_type, &value);

      thread_work[i][j].type = work_type;
      thread_work[i][j].value = value;
    }
  }

  fclose(fp);

  // set random seed
  srand(time(NULL));

  // initialize list
  list_init(&head, &tail);

  pthread_t threads[MAX_NUM_THREAD];

  // create threads
  for (long i = 0; i < num_thread; i++) {
    if (pthread_create(&threads[i], NULL, ThreadFunc, (void *)i) < 0) {
      exit(0);
    }
  }

  // wait threads end
  for (long i = 0; i < num_thread; i++) {
    pthread_join(threads[i], NULL);
  }

  // validate list
  FILE *fp_result = fopen("result.txt", "r");
  int size_result;
  int value_result;

  fscanf(fp_result, "%d", &size_result);

  bool is_valid = true;
  Node *it = head->next;
  for (int i = 0; i < size_result; i++) {
    if (it == tail) {
      is_valid = false;
      break;
    }
    fscanf(fp_result, "%d", &value_result);
    if (it->key != value_result) {
      is_valid = false;
      break;
    }
    it = it->next;
  }
  if (it != tail) {
    is_valid = false;
  }

  fclose(fp_result);

  if (is_valid) {
    printf("correct!\n");
  } else {
    printf("incorrect!\n");
  }

  return 0;
}
