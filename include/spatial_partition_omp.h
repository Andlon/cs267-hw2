#ifndef __CS267_SPATIAL_PARTITION_OMP_H
#define __CS267_SPATIAL_PARTITION_OMP_H

#include "spatial_partition.h"

// Updates forces for a subset of the particles in storage, specified by the vector of particles
void update_forces_omp(std::vector<particle_t> & particles, partitioned_storage &storage,
                       const grid &grid, double *dmin, double *davg, int *navg);

// Updates forces for all particles in storage
void update_forces_omp(partitioned_storage &storage, const grid & grid, double *dmin, double *davg, int *navg);

// Move particles
void move_particles_omp(std::vector<particle_t> & particles);

// Move all particles in storage
void move_particles_omp(partitioned_storage &storage);

#endif // __CS267_SPATIAL_PARTITION_OMP_H

