
all:

include ../particle.mk

CXX_SRCS := serial.cpp serial_default.cpp

BINFILES = $(CXX_SRCS:.cpp=)

OBJFILES = $(CXX_SRCS:.cpp=.o)

ADDHEADER = ../util/common.h

ADDLIBS = ../util/common.o

include ../default_rules.mk
