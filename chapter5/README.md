# Evaluation
## Environment
### HW
```
Processor    : Apple M1 chip
Configuration: 8-core CPU with 4 perform­ance cores and 4 efficiency cores
Main Memory  : 8GB unified memory
Storage      : APPLE SSD AP0256Q 256GB
```
### SW
```
OS: macOS Monterey v12.6
clang: Apple clang version 13.1.6 (clang-1316.0.21.2.5)
```
## How to build
```sh
make
```
## How to execute
```sh
./bin/{Program} < workload.txt
```
Change `{Program}` for each code.

## Results
### flag
```
[61632 / 61632]
Fails: 75
```
> Consensus number: 1
### queue
```
[61632 / 61632]
Fails: 246
```
> Consensus number: 1
### multi_assign
```
[61632 / 61632]
Fails: 0
```
> Consensus number: 2
### flag_atomic (get_and_fetch)
```
[61632 / 61632]
Fails: 0
```
> Consensus number: 2