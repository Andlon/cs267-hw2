
all:

include ../particle.mk

CXX_SRCS := openmp_alternative.cpp

BINFILES = $(CXX_SRCS:.cpp=)

OBJFILES = $(CXX_SRCS:.cpp=.o)

ADDHEADER = ../util/common_alternative.h ../util/spatial_partition.h ../util/spatial_partition_omp.h ../util/grid.h

ADDLIBS = ../util/common_alternative.o ../util/spatial_partition.o ../util/spatial_partition_omp.o ../util/grid.o

CFLAGS += $(OPENMP)

LFLAGS += $(OPENMP)

include ../default_rules.mk
