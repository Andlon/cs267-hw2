
all:

include ../particle.mk

CXX_SRCS := common.cpp spatial_partition.cpp spatial_partition_omp.cpp common_alternative.cpp grid.cpp

OBJFILES := $(CXX_SRCS:.cpp=.o)

BINFILES = 

ADDHEADER = common_alternative.h spatial_partition.h spatial_partition_omp.h grid.h

grid.o: common_alternative.o
spatial_partition.o: grid.o
spatial_partition_omp.o: spatial_partition.o

include ../default_rules.mk
