# Makefile

CC_GCC = gcc
CC_ICC = icc
MPI_CC_GCC = mpicc
MPI_CC_ICC = mpiicc
CFLAGS = -O3 -Wall
LFLAGS_GCC = -fopenmp
LFLAGS_ICC = -qopenmp

# Targets
gccserial: main-serial.c
	$(CC_GCC) $(LFLAGS_GCC) $^ -o stencil-omp-gcc.exe -lm

gcccomplete: main-mpi.c stencil.c
	$(MPI_CC_GCC) $(CFLAGS) $^ -o stencil-complete-gcc.exe

iccserial: main-serial.c stencil.c
	$(CC_ICC) $(CFLAGS) $(LFLAGS_ICC) $^ -o stencil-omp-icc.exe

icccomplete: main-mpi.c stencil.c
	$(MPI_CC_ICC) $(CFLAGS) $^ -o stencil-complete-icc.exe
