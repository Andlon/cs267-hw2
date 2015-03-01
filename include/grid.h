#ifndef __CS267_GRID_H
#define __CS267_GRID_H

#include <vector>
#include <cstdio>
#include "common.h"

typedef std::vector<particle_t *> particle_bin;

// Returns true if x is in the (half-open) interval [a, b).
// (Yeah, it doesn't really belong here semantically speaking)
inline bool in_range(int x, int a, int b)
{
    return a <= x && x < b;
}

class grid {
public:
    grid(double size, double minimum_bin_size);

    const size_t bins_per_dimension;
    const size_t binsize_per_dimension;
    const size_t size;

    size_t bincount() const;

    particle_bin & operator() (int x, int y);
    particle_bin & operator[] (size_t bin_id);

    void clear_particles();
    void distribute_particles(particle_t * particles, size_t count);

private:
    std::vector<particle_bin> bins;
};

void calculate_forces_for_grid(grid & grid, double *dmin, double *davg, int *navg);
void calculate_forces_for_bin(grid & grid, size_t bin_id, double *dmin, double *davg, int *navg);
void calculate_forces_for_particle(grid & grid, size_t bin_id, particle_t & particle, double *dmin, double *davg, int *navg);
size_t compute_bin_for_particle(const grid & grid, const particle_t & t);

#endif //__CS267_GRID_H

