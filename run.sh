#!/bin/bash -l

#SBATCH -p course -t 2
#SBATCH -D ./
#SBATCH --export=ALL

# load modules intel compiler
module load compilers/intel/2019u5
module load mpi/intel-mpi/2019u5/bin

make gccserial
make gcccomplete
make iccserial
make icccomplete

threads=(1 2 4 8 16 32)

for i in "${threads[@]}"
do
    echo "Runnin gcc OpenMP stencil with $i threads:"
    export OMP_NUM_THREADS=$i
    ./stencil-omp-gcc.exe
    echo "---"

done

# for i in "${threads[@]}"
# do
#     echo "Runnin icc OpenMP stencil with $i threads:"
#     export OMP_NUM_THREADS=$i
#     ./stencil-omp-icc.exe
#     echo "---"

# done

# for i in "${threads[@]}"
# do
#     echo "Runnin gcc MPI stencil with $i ranks:"
#     mpiexec -n $i ./stencil-complete-gcc.exe
#     echo "---"

# done

# for i in "${threads[@]}"
# do
#     echo "Runnin icc MPI stencil with $i ranks:"
#     mpiexec -n $i ./stencil-complete-icc.exe
#     echo "---"

# done
