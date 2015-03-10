CC = g++
MPCC = mpic++
OPENMP = -fopenmp -D_GLIBCXX_PARALLEL
CFLAGS = -O3 -I$(INCLUDEPATH) -std=c++11 -march=native
DEBUG_CFLAGS = -g -O0 -fno-inline -I$(INCLUDEPATH) -std=c++11
LIBS = -lm
