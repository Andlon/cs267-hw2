CC = g++
MPCC = mpic++
OPENMP = -fopenmp
RELEASE_CFLAGS = -O3 -I$(INCLUDEPATH) -std=c++11 -march=native -DNDEBUG -Wno-literal-suffix 
DEBUG_CFLAGS = -g -O0 -fno-inline -I$(INCLUDEPATH) -std=c++11
LIBS = -lm
