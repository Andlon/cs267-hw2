# Build Homework 2 on Hopper

# Define include path
INCLUDEPATH = include
BINDIR = bin
BUILDDIR = build
SRC = src

CC = CC
MPCC = CC
OPENMP = -mp
CFLAGS = -O3 -I$(INCLUDEPATH)
LIBS = 

# Define variables for the different targets to encompass build directory
SERIAL_TARGET = $(BINDIR)/serial
PTHREADS_TARGET = $(BINDIR)/pthreads
OPENMP_TARGET = $(BINDIR)/openmp
MPI_TARGET = $(BINDIR)/mpi
AUTOGRADER_TARGET = $(BINDIR)/autograder

TARGETS = $(SERIAL_TARGET) $(PTHREADS_TARGET) $(OPENMP_TARGET) $(MPI_TARGET) $(AUTOGRADER_TARGET)

all:	$(TARGETS)

$(SERIAL_TARGET): $(BUILDDIR)/serial.o $(BUILDDIR)/common.o
	$(CC) -o $@ $(LIBS) $(BUILDDIR)/serial.o $(BUILDDIR)/common.o

$(AUTOGRADER_TARGET): $(BUILDDIR)/autograder.o $(BUILDDIR)/common.o
	$(CC) -o $@ $(LIBS) $(BUILDDIR)/autograder.o $(BUILDDIR)/common.o

$(PTHREADS_TARGET): $(BUILDDIR)/pthreads.o $(BUILDDIR)/common.o
	$(CC) -o $@ $(LIBS) -lpthread $(BUILDDIR)/pthreads.o $(BUILDDIR)/common.o

$(OPENMP_TARGET): $(BUILDDIR)/openmp.o $(BUILDDIR)/common.o
	$(CC) -o $@ $(LIBS) $(OPENMP) $(BUILDDIR)/openmp.o $(BUILDDIR)/common.o

$(MPI_TARGET): $(BUILDDIR)/mpi.o $(BUILDDIR)/common.o
	$(MPCC) -o $@ $(LIBS) $(MPILIBS) $(BUILDDIR)/mpi.o $(BUILDDIR)/common.o

# Build all object files (note: here we assume we want to use the same compiler flags for all objects)
$(BUILDDIR)/%.o: $(SRC)/%.cpp
	$(CC) -c $(CFLAGS) $^

clean:
	rm -f *.o $(TARGETS) *.stdout *.txt
