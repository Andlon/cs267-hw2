#ifndef __CS267_GRID_H
#define __CS267_GRID_H

#include <vector>
#include <cstdio>
#include "common.h"

// Returns true if x is in the (half-open) interval [a, b).
// (Yeah, it doesn't really belong here in this header semantically speaking)
inline bool in_range(int x, int a, int b)
{
    return a <= x && x < b;
}

class grid {
public:
    grid(double gridsize, double minimum_partition_size);
    grid(size_t particle_count, double gridsize, double minimum_partition_size);

    // Total number of partitions (bins_per_dim()^2)
    size_t partition_count() const;

    // Number of partitions in one dimension
    size_t partitions_per_dim() const;

    // Physical size of an individual partition in one dimension.
    double partition_size() const;

    // Physical size of grid in one dimension.
    double size() const;

    // Number of particles on grid
    size_t particle_count() const;

private:
    size_t _partitions_per_dim;
    size_t _particle_count;
    double _partition_size;
    double _size;
};

size_t determine_partition_for_particle(const grid & grid, const particle_t & t);

#endif //__CS267_GRID_H

