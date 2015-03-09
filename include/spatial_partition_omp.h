#ifndef __CS267_SPATIAL_PARTITION_OMP_H
#define __CS267_SPATIAL_PARTITION_OMP_H

#include "spatial_partition.h"

void update_forces_omp(partitioned_storage &storage, const grid & grid, double *dmin, double *davg, int *navg);

void move_particles_omp(partitioned_storage &storage);

#endif // __CS267_SPATIAL_PARTITION_OMP_H

