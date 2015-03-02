
all:

include ../particle.mk

CXX_SRCS := openmp.cpp

BINFILES = $(CXX_SRCS:.cpp=)

OBJFILES = $(CXX_SRCS:.cpp=.o)

ADDHEADER = ../util/common.h

ADDLIBS = ../util/common.o

CFLAGS += $(OPENMP)

LFLAGS += $(OPENMP)

include ../default_rules.mk
