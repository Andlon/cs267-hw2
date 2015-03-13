
SHELL := /bin/bash

SUBDIRS = util serial serial_alternative openmp openmp_alternative pthreads mpi mpi_alternative grader gpu
SUBDIRS_NOGPU = $(filter-out gpu,$(SUBDIRS))

all: $(SUBDIRS)
	-echo Done

# Builds everything except gpu (useful when compiler not available)
nogpu: $(SUBDIRS_NOGPU)
	@echo =======================
	@echo Completed: nogpu build.
	
.PHONY: $(SUBDIRS)
$(SUBDIRS) :
	$(MAKE) -C $@

.PHONY: $(SUBDIRS_NOGPU)
$(SUBDIRS_NOGPU):
	$(MAKE) -C $@

clean:
	-for x in $(SUBDIRS); do $(MAKE) -C $$x clean; done
