# Count Prime Numbers
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

# Love Story of Alice & Bob
## mutex1
Alice's pet and Bob's pet must be mutually exclusive.
Only 1 pet can get in the pond.
So they made the *coordination protocol*.
```
1. Raise flag
2. Wait until the other’s flag is down
3. Unleash pet
4. Lower flag when pet returns
```
## mutex2
### Alice's protocol
```
1. She raises her flag.
2. When Bob’s flag is lowered, she unleashes her cat.
3. When her cat comes back, she lowers her flag.
```
### Bob's protocol
```
  1. He raises his flag.
  2. While Alice’s flag is raised
    a) Bob lowers his flag
    b) Bob waits until Alice’s flag is lowered
    c) Bob raises his flag
  3. As soon as his flag is raised and hers is down, he unleashes his dog.
  4. When his dog comes back, he lowers his flag.
```

## producer_consumer
### Alice's protocol
```
1. She waits until the can is down.
2. She releases the pets.
3. When the pets return, Alice checks whether they finished the food. If so, she resets the can.
```
### Bob's protocol
```
1. He waits until the can is up.
2. He puts food in the yard.
3. He pulls the string and knocks the can down.
```

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
## Results: count prime numbers
|Program|Count|Time (s)|
|---|---|---|
|sequential|664580|3.94703|
|figure1|664580|0.874875|
|figure2|891212|0.942655|
|swarm|664580|1.08075|
|atomic|664580|0.786369|

## Results: Alice and Bob
|Program|Deadlock|Checksum|Alice's duration|Bob's duration|
|---|---|---|---|---|
|mutex1|O|-|-|-|
|mutex2|X|2000000|0.034231s|0.039176s|
|producer_consumer|X|0|0.112907s|0.112873s|
