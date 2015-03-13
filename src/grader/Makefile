
all:

include ../particle.mk

CXX_SRCS := autograder.cpp

BINFILES = $(CXX_SRCS:.cpp=)

OBJFILES = $(CXX_SRCS:.cpp=.o)

ADDHEADER = ../util/common.h

ADDLIBS = ../util/common.o

# Ignore any warnings from building autograder
CFLAGS = -w

include ../default_rules.mk
