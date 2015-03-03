
all:

include ../particle.mk

CXX_SRCS := gpu.cu

BINFILES = $(CXX_SRCS:.cu=)

OBJFILES = $(CXX_SRCS:.cu=.o)

ADDHEADER = ../util/common.h

ADDLIBS = ../util/common.o

CC = $(NVCC)

CFLAGS += -arch=sm_35

include ../default_rules.mk
