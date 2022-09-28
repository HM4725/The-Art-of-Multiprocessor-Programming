# Description
## sequential
Get the number of prime numbers from 1 to 10,000,000 sequentially.

## figure1
Balancing load by dividing up the input domain. Each thread in {0..NTHREADS-1} gets an equal subset of the range.

![figure1](./img/figure1.1.svg)

## figure2
Balancing the workload using a shared counter. Each thread gets a dynamically determined number of numbers to test.

![figure2](./img/figure1.2.svg)

## swarm
Similar with figure1. But different at loop.
```c
// figure1
long block = limit / NTHREADS;
for (int n = (id * block) + 1; n <= (id + 1) * block; n++) {
    if (isPrime(n)) {
        count += 1;
    }
}
```
```c
// swarm
for (int n = id + 1; n <= limit; n += NTHREADS) {
    if (isPrime(n)) {
        count += 1;
    }
}
```

![swarm](./img/swarm.svg)

## atomic
Atomic version of figure2.

# Evaluation
## How to build
```sh
make
```
## How to execute
```sh
./bin/{Program}
```
Change `{Program}` for each code.
## Environment
```
Processor    : Apple M1 chip
Configuration: 8-core CPU with 4 perform­ance cores and 4 efficiency cores
Main Memory  : 8GB unified memory
Storage      : APPLE SSD AP0256Q 256GB
```
## Results
|Program|Count|Time (s)|
|---|---|---|
|sequential|664580|3.94703|
|figure1|664580|0.874875|
|figure2|891212|0.942655|
|swarm|664580|1.08075|
|atomic|664580|0.786369|