
SHELL := /bin/bash

SUBDIRS = util serial serial_alternative openmp openmp_alternative pthreads mpi mpi_alternative grader gpu

all: $(SUBDIRS)
	-echo Done

# Builds everything except gpu (useful when compiler not available)
nogpu: $(filter-out gpu,$(SUBDIRS) )
	@echo =======================
	@echo Completed: nogpu build.
	
.PHONY: $(SUBDIRS)
$(SUBDIRS) :
	$(MAKE) -C $@

clean:
	-for x in $(SUBDIRS); do $(MAKE) -C $$x clean; done
