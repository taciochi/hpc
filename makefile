CC_GCC = gcc
CC_ICC = icc
MPI_CC_GCC = mpicc
MPI_CC_ICC = mpiicc
LFLAGS_GCC = -fopenmp
LFLAGS_ICC = -qopenmp

# Targets
gccserial: main-serial.c
	$(CC_GCC) $(LFLAGS_GCC) $^ -o stencil-omp-gcc.exe -lm

gcccomplete: main-mpi.c
	$(MPI_CC_GCC) $^ -o stencil-complete-gcc.exe

iccserial: main-serial.c
	$(CC_ICC) $(LFLAGS_ICC) $^ -o stencil-omp-icc.exe

icccomplete: main-mpi.c
	$(MPI_CC_ICC) $^ -o stencil-complete-icc.exe