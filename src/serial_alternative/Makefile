
all:

include ../particle.mk

CXX_SRCS := serial_alternative.cpp

BINFILES = $(CXX_SRCS:.cpp=)

OBJFILES = $(CXX_SRCS:.cpp=.o)

ADDHEADER = ../util/common_alternative.h ../util/grid.h ../util/spatial_partition.h

ADDLIBS =  ../util/spatial_partition.o ../util/grid.o ../util/common_alternative.o

serial_alternative.o: $(ADDLIBS)

include ../default_rules.mk
