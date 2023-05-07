#!/bin/bash -l

#SBATCH -p course -t 2
#SBATCH -D ./
#SBATCH --export=ALL

# load modules intel compiler
module load compilers/intel/2019u5 

make -f ./makefile iccserial
make -f ./makefile gccserial

threads=(1 2 4 8 12 16 32)

for thread_conunt in "${threads[@]}"
do
    echo "Runnin gcc OpenMP stencil with $thread_count threads:"
    export OMP_NUM_THREADS=$thread_conunt
    ./stencil-omp-gcc.exe

done
