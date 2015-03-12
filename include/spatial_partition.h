#ifndef __CS267_SPATIAL_PARTITION_H
#define __CS267_SPATIAL_PARTITION_H

#include <vector>
#include <cstdlib>
#include "common.h"
#include "grid.h"

// Stores a partitioned collection of particles. Storage format
// similar to CSR for sparse matrices.
struct partitioned_storage {
    explicit partitioned_storage(const grid & particle_grid);

    // Holds the particles, sorted such that for two particles a and b,
    // where a comes before b in the set,
    // partition(a) <= particle(b),
    // assuming partition(storage, grid) has been called
    std::vector<particle_t> particles;

    // size of 'partitions' is equal to number of partitions.
    // The value of every element corresponds to the index where the
    // next partition begins. As an example,
    // partitions[0] = 27;
    // partitions[1] = 34;
    // would mean that the particles in partition 0 can be found in
    // particles[0], ..., particles[26]
    // and the particles in partition 1 can be found in
    // particles[27], ..., particles[33]
    std::vector<size_t> partitions;
};

// Updates the partition property of each particle to match its location
void update_partitions(std::vector<particle_t> & particles, const grid & grid);

// (Re-)partitions the particles in the storage according to their locations
// and properties of the grid.
void partition(partitioned_storage & storage, const grid & grid);

// Updates forces for a subset of the particles in storage, specified by the vector of particles.
// Assumes particles are correctly partitioned.
void update_forces(std::vector<particle_t> & particles, partitioned_storage &storage,
                       const grid &grid, double *dmin, double *davg, int *navg);

// Updates forces for all particles in the storage. Assumes particles are correctly partitioned.
void update_forces(partitioned_storage & storage, const grid & grid, double *dmin, double *davg, int *navg);

void compute_particle_forces(partitioned_storage &storage, const grid & grid, particle_t &particle,
                             double *dmin, double *davg, int *navg);

void compute_particle_forces_in_partition(partitioned_storage & storage, particle_t & particle, size_t partition_index,
                                          double *dmin, double *davg, int *navg);


void move_particles(std::vector<particle_t> & particles);
void move_particles(partitioned_storage & storage);

#endif // __CS267_SPATIAL_PARTITION_H

