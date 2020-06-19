# Multiprocessing grep
Creating a simple grep using multiprocessing api (OpenMP). ***It's worse than normal grep, so don't use it.***

## Prerequisite
- Python (2 or 3)
- g++

## Usage
### Compile worker
`g++ -fopenmp workers/openmp.cpp -o sf`
### Run stringfinder
`python <path_to_main.py> <number_of_processes> <number_of_threads> <pattern>`