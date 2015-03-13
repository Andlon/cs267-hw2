
SHELL := /bin/bash

SUBDIRS = util serial openmp pthreads mpi grader gpu

all: $(SUBDIRS)
	-echo Done
	
.PHONY: $(SUBDIRS)
$(SUBDIRS) :
	$(MAKE) -C $@

clean:
	-for x in $(SUBDIRS); do $(MAKE) -C $$x clean; done
