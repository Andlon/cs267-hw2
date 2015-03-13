
CC = g++
MPCC = mpic++
NVCC = nvcc
OPENMP = -fopenmp
CFLAGS = -O3 -std=c++11 -march=native -DNDEBUG -Wno-literal-suffix 
#CFLAGS = -O0 -std=c++11 -march=native -DNDEBUG -Wno-literal-suffix -g -fno-inline
LFLAGS = -lm
