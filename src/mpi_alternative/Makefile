
all:

include ../particle.mk

CXX_SRCS := mpi_alternative.cpp

BINFILES = $(CXX_SRCS:.cpp=)

OBJFILES = $(CXX_SRCS:.cpp=.o)

ADDHEADER = ../util/common.h ../util/spatial_partition_omp.h ../util/grid.h

ADDLIBS = ../util/common_alternative.o ../util/grid.o ../util/spatial_partition.o ../util/spatial_partition_omp.o

CC = $(MPCC)

CFLAGS += $(OPENMP)
LFLAGS += $(OPENMP)

include ../default_rules.mk
