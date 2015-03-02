CC = g++
MPCC = mpic++
OPENMP = -fopenmp
CFLAGS = -O3 -I$(INCLUDEPATH) -std=c++11
DEBUG_CFLAGS = -g -O0 -fno-inline -I$(INCLUDEPATH) -std=c++11
LIBS = -lm
