
all:

include ../particle.mk

CXX_SRCS := mpi.cpp mpi_naive.cpp

BINFILES = $(CXX_SRCS:.cpp=)

OBJFILES = $(CXX_SRCS:.cpp=.o)

ADDHEADER = ../util/common.h

ADDLIBS = ../util/common.o

CC = $(MPCC)

include ../default_rules.mk
