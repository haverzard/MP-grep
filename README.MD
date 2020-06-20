# Multiprocessing grep
Creating a simple grep using multiprocessing api (OpenMP). ***It's worse than normal grep, so don't use it.***

## Prerequisite
- g++
- Linux Operating System

## Usage
### Compile worker
`g++ -O3 -fopenmp workers/openmp.cpp -o sf`
### Run stringfinder
`./<path_to_sf> <number_of_processes> <number_of_threads> <pattern>`