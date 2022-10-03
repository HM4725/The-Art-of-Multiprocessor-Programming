# Mutex: 2-Thread Solutions
## lock1
2.3.1 The LockOne Class (page 25)

## lock2
2.3.2 The LockTwo Class (page 26)

## peterson
2.3.3 The Peterson Lock (page 27)

## lockfree
*NOT IN TEXTBOOK*

Design with the atomic instruction.

## sloppy
*NOT IN TEXTBOOK*

Design with the sloppy counter.


# Mutex: N-Thread Solutions
## filter

## bakery

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
## Results: 2-Thread Solutions
|Program|Deadlock|Checksum|duration1|duration2|
|---|---|---|---|---|
|lock1|O|-|-|-|
|lock2|O|-|-|-|
|peterson|X|2000000|0.191351s|0.191338s|
|lockfree|X|2000000|0.054978s|0.059308s|
|sloppy|X|2000000|0.009734s|0.009746s|

## Results: N-Thread Solutions
|Program|Deadlock|Checksum|duration0|duration1|duration2|duration3|duration4|duration5|duration6|duration7|
|---|---|---|---|---|---|---|---|---|---|---|
|filter|X|800000|0.680179s|0.678443s|0.677037s|0.680617s|0.680461s|0.673903s|0.680649s|0.677445s|