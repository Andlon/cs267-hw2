
all:

include ../particle.mk

CXX_SRCS := pthreads.cpp

BINFILES = $(CXX_SRCS:.cpp=)

OBJFILES = $(CXX_SRCS:.cpp=.o)

CFLAGS += -pthread

LFLAGS += -pthread

ADDHEADER = ../util/common.h

ADDLIBS = ../util/common.o

include ../default_rules.mk
