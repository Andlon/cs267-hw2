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
SRC = src

# Define variables for the different targets to encompass output directory
SERIAL_TARGET = $(BINDIR)/serial
PTHREADS_TARGET = $(BINDIR)/pthreads
OPENMP_TARGET = $(BINDIR)/openmp
MPI_TARGET = $(BINDIR)/mpi
AUTOGRADER_TARGET = $(BINDIR)/autograder

TARGETS = $(SERIAL_TARGET) $(PTHREADS_TARGET) $(OPENMP_TARGET) $(MPI_TARGET) $(AUTOGRADER_TARGET)

# Define build targets
all:	$(TARGETS)

debug: 	CFLAGS = $(DEBUG_CFLAGS)
debug: $(TARGETS)

pre-build:
	mkdir -p build
	mkdir -p bin

$(SERIAL_TARGET): $(BUILDDIR)/grid.o $(BUILDDIR)/serial.o $(BUILDDIR)/common.o 
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@  

$(AUTOGRADER_TARGET): $(BUILDDIR)/autograder.o $(BUILDDIR)/common.o
	$(CC) -o $@ $(BUILDDIR)/autograder.o $(BUILDDIR)/common.o $(LIBS) 

$(PTHREADS_TARGET): $(BUILDDIR)/pthreads.o $(BUILDDIR)/common.o
	$(CC) -o $@ $(BUILDDIR)/pthreads.o $(BUILDDIR)/common.o $(LIBS) -lpthread 

$(OPENMP_TARGET): $(BUILDDIR)/openmp.o $(BUILDDIR)/common.o
	$(CC) -o $@ $(OPENMP) $(BUILDDIR)/openmp.o $(BUILDDIR)/common.o $(LIBS) 

$(MPI_TARGET): $(BUILDDIR)/mpi.o $(BUILDDIR)/common.o
	$(MPCC) -o $@ $(MPILIBS) $(BUILDDIR)/mpi.o $(BUILDDIR)/common.o $(LIBS) 

# Build all object files (note: here we assume we want to use the same compiler flags for all objects)
# We add pre-build as a dependency and filter it out when building as to make sure necessary directories exist
# mpi is treated separately as we need to use the MPI compiler
$(BUILDDIR)/mpi.o: $(SRC)/mpi.cpp pre-build
	$(MPCC) -c $(CFLAGS) $(filter-out pre-build,$^) -o $@

$(BUILDDIR)/%.o: $(SRC)/%.cpp pre-build	
	$(CC) -c $(CFLAGS) $(filter-out pre-build,$^) -o $@

clean:
	rm -f -R $(BUILDDIR)
	rm -f *.o $(BINDIR)/*.stdout
