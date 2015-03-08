# Build Homework 2 on Hopper

# Include compiler-specifics (gcc or CC)
ifdef COMPILER
include $(COMPILER).make
else
include CC.make
endif

# Define include path
INCLUDEPATH = include
BINDIR = bin
BUILDDIR = build
TESTDIR = test
SRC = src

# Define variables for the different targets to encompass output directory
SERIAL_TARGET = $(BINDIR)/serial
PTHREADS_TARGET = $(BINDIR)/pthreads
OPENMP_TARGET = $(BINDIR)/openmp
MPI_TARGET = $(BINDIR)/mpi
AUTOGRADER_TARGET = $(BINDIR)/autograder

TARGETS = $(SERIAL_TARGET) $(PTHREADS_TARGET) $(OPENMP_TARGET) $(MPI_TARGET) $(AUTOGRADER_TARGET)

# Define variables for the tests
GRIDTEST = $(BINDIR)/test/gridtest
TESTS = $(GRIDTEST)

# Define build targets
all:	main-build
main-build: pre-build $(TARGETS)
debug: CFLAGS = $(DEBUG_CFLAGS)
debug: main-build
debug-tests: debug tests
tests: pre-build $(TESTS)
check: 	tests
	./$(BINDIR)/test/gridtest
pre-build:
	mkdir -p build/test
	mkdir -p bin/test
	mkdir -p test


# Targets below
$(SERIAL_TARGET): $(BUILDDIR)/grid.o $(BUILDDIR)/spatial_partition.o $(BUILDDIR)/serial.o $(BUILDDIR)/common.o 
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@  

$(AUTOGRADER_TARGET): $(BUILDDIR)/autograder.o $(BUILDDIR)/common.o
	$(CC) -o $@ $(BUILDDIR)/autograder.o $(BUILDDIR)/common.o $(LIBS) 

$(PTHREADS_TARGET): $(BUILDDIR)/pthreads.o $(BUILDDIR)/common.o
	$(CC) -o $@ $(BUILDDIR)/pthreads.o $(BUILDDIR)/common.o $(LIBS) -lpthread 

$(OPENMP_TARGET): $(BUILDDIR)/openmp.o $(BUILDDIR)/common.o
	$(CC) -o $@ $(OPENMP) $(BUILDDIR)/openmp.o $(BUILDDIR)/common.o $(LIBS) 

$(MPI_TARGET): $(BUILDDIR)/grid.o $(BUILDDIR)/mpi.o $(BUILDDIR)/common.o
	$(MPCC) $(CFLAGS) $^ $(LIBS) $(MPILIBS) -o $@

# Tests
$(GRIDTEST): $(BUILDDIR)/spatial_partition.o  $(BUILDDIR)/common.o $(BUILDDIR)/grid.o $(BUILDDIR)/test/catch.o $(BUILDDIR)/test/gridtest.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# Build all object files (note: here we assume we want to use the same compiler flags for all objects)
# We add pre-build as a dependency and filter it out when building as to make sure necessary directories exist
# mpi is treated separately as we need to use the MPI compiler

$(BUILDDIR)/common.o: $(SRC)/common.cpp include/common.h	
	$(CC) $(CFLAGS) -c $(SRC)/common.cpp -o $@

$(BUILDDIR)/grid.o: $(SRC)/grid.cpp include/grid.h	
	$(CC) $(CFLAGS) -c $(SRC)/grid.cpp -o $@

$(BUILDDIR)/spatial_partition.o: $(SRC)/spatial_partition.cpp include/spatial_partition.h
	$(CC) $(CFLAGS) -c $(SRC)/spatial_partition.cpp -o $@

$(BUILDDIR)/pthread_barrier.o: $(SRC)/pthread_barrier.c include/pthread_barrier.h
	$(CC) $(CFLAGS) -c $(SRC)/pthread_barrier.c -o $@

$(BUILDDIR)/mpi.o: $(SRC)/mpi.cpp
	$(MPCC) $(CFLAGS) -c $? $(LIBS) $(MPILIBS) -o $@

$(BUILDDIR)/%.o: $(SRC)/%.cpp
	$(CC) $(CFLAGS) -c $? -o $@


# Object files for tests
$(BUILDDIR)/test/catch.o: $(TESTDIR)/catch_precompile.cpp
	$(CC) -c $(CFLAGS) $? -o $@

$(BUILDDIR)/test/%.o: $(TESTDIR)/%.cpp
	$(CC) -c $(CFLAGS) $? -o $@

clean:
	rm -f -R $(BUILDDIR)
	rm -f *.o $(BINDIR)/*.stdout
