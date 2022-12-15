Chapter9) Linked Lists: The Role of Locking
===============
## Concurrent Design Patterns
In this chapter, it designs 4 patterns with list-based set.
### 1. Coarse Grained Synchronization
`CoarseGrainedSet`
### 2. Fine Grained Synchronization
`FineGrainedSet`
### 3. Optimistic Synchronization
`OptimisticSet`
### 4. Lazy Synchronization
`LazySyncSet`
### 5. Lock Free Synchronization
`LockFreeSet`

## Evaluation
### Build
```sh
make -j
```
### Run
```sh
time ./bin/${prog}
```
