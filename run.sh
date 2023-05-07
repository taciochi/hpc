#!/bin/bash -l

#SBATCH -p course -t 2
#SBATCH -D ./
#SBATCH --export=ALL

# load modules intel compiler
module load compilers/intel/2019u5
module load mpi/intel-mpi/2019u5/bin

make gccserial
# make gcccomplete
make iccserial
# make icccomplete

threads=(1 2 4 8 12 16 32)

for thread_conunt in "${threads[@]}"
do
    echo "Runnin gcc OpenMP stencil with $thread_count threads:"
    export OMP_NUM_THREADS=$thread_conunt
    ./stencil-omp-gcc.exe
    echo "---"

done

for thread_conunt in "${threads[@]}"
do
    echo "Runnin icc OpenMP stencil with $thread_count threads:"
    export OMP_NUM_THREADS=$thread_conunt
    ./stencil-omp-icc.exe
    echo "---"

done

# for thread_conunt in "${threads[@]}"
# do
#     echo "Runnin gcc MPI stencil with $thread_count ranks:"
#     mpiexec -n "$thread_count" ./stencil-complete-gcc.exe
#     echo "---"

# done
