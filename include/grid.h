#ifndef __CS267_GRID_H
#define __CS267_GRID_H

#include <vector>
#include <cstdio>
#include "common.h"

struct particle_bin {
    // Note that this bin itself is included in its own neighborhood
    std::vector<size_t>         neighbors;
    std::vector<particle_t *>   particles;
};

// Returns true if x is in the (half-open) interval [a, b).
// (Yeah, it doesn't really belong here semantically speaking)
inline bool in_range(int x, int a, int b)
{
    return a <= x && x < b;
}

class grid {
public:
    grid(double gridsize, double minimum_bin_size);

    // Total number of bins (bins_per_dim()^2)
    size_t bincount() const;

    // Number of bins in one dimension
    size_t bins_per_dim() const;

    // Physical size of an individual bin in one dimension.
    double binsize() const;

    // Physical size of grid in one dimension.
    double size() const;

    void clear_particles();
    void distribute_particles(particle_t * particles, size_t count);

    particle_bin & operator[] (size_t bin_id);
    const particle_bin & operator[] (size_t bin_id) const;

private:
    size_t _bins_per_dim;
    double _binsize;
    double _size;

    std::vector<particle_bin> _bins;
};

void compute_forces_for_grid(grid & grid, double *dmin, double *davg, int *navg);
void compute_forces_for_bin(grid & grid, size_t bin_id, double *dmin, double *davg, int *navg);
void compute_forces_for_particle(grid & grid, size_t bin_id, particle_t & particle, double *dmin, double *davg, int *navg);
size_t determine_bin_for_particle(const grid & grid, const particle_t & t);

std::vector<size_t> acquire_interacting_bins(const grid & grid, size_t bin_id);

#endif //__CS267_GRID_H

