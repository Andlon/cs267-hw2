#include <grid.h>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <array>

namespace {

inline int computeIndex(int x, int y, int width) {
    return x + width * y;
}

int computeNumberOfBins(double size, double minimum_bin_size) {
    assert(size > 0);
    assert(minimum_bin_size > 0);
    minimum_bin_size = std::min(size, minimum_bin_size);
    return static_cast<int> (size / minimum_bin_size);
}

}

std::vector<size_t> acquire_interacting_bins(const grid & grid, size_t bin_id) {
    assert(bin_id < grid.bincount());

    int x0 = ((int) bin_id) % grid.bins_per_dimension;
    int y0 = ((int) bin_id) / grid.bins_per_dimension;

    // All coordinates of neighboring bins, including this bin
    std::array<std::pair<int, int>, 9> bins_xy = {{
        { x0 - 1, y0 - 1 },
        { x0,     y0 - 1 },
        { x0 + 1, y0 - 1 },
        { x0 - 1, y0     },
        { x0,     y0     },
        { x0 + 1, y0     },
        { x0 - 1, y0 + 1 },
        { x0,     y0 + 1 },
        { x0 + 1, y0 + 1 }
    }};

    // Transform coordinates to indices, filtering out non-existent bins
    std::vector<size_t> bin_ids;
    for (const auto & coord : bins_xy)
    {
        int x = coord.first;
        int y = coord.second;
        if (    in_range(x, 0, grid.bins_per_dimension) &&
                in_range(y, 0, grid.bins_per_dimension))
        {
            size_t id = static_cast<size_t> (coord.first + coord.second * grid.bins_per_dimension);
            bin_ids.push_back(id);
        }
    }

    return bin_ids;
}

// Note:
// avoid using bins_per_dimension in the definition of
// binsize_per_dimension, as if the definition in the header file
// is accidentally reordered, it may introduce an insidious bug. Hence,
// we use the computeNumberOfBins function to compute every time.
grid::grid(double gridsize, double minimum_bin_size)
    : bins_per_dimension(computeNumberOfBins(gridsize, minimum_bin_size)),
      binsize_per_dimension( gridsize / computeNumberOfBins(gridsize, minimum_bin_size) ),
      size(gridsize)
{
    bins.resize(bincount());
}

size_t grid::bincount() const
{
    return bins_per_dimension * bins_per_dimension;
}

particle_bin &grid::operator()(int x, int y) {
    assert(x >= 0);
    assert(x < bins_per_dimension);
    assert(y >= 0);
    assert(y < bins_per_dimension);
    return bins[x + bins_per_dimension * y];
}


particle_bin &grid::operator[](size_t bin_id) {
    assert(bin_id < bins.size());
    return bins[bin_id];
}

void grid::clear_particles()
{
    for (auto & bin : bins)
        bin.clear();
}

void grid::distribute_particles(particle_t *particles, size_t count)
{
    for (size_t i = 0; i < count; ++i) {
        particle_t * particle = particles + i;
        size_t bin = determine_bin_for_particle(*this, *particle);
        bins[bin].push_back(particle);
    }
}


void compute_forces_for_bin(grid &grid, size_t bin_id, double *dmin, double *davg, int *navg)
{
    assert(in_range(bin_id, 0, grid.bincount()));
    for (auto particle_pointer : grid[bin_id]) {
        compute_forces_for_particle(grid, bin_id, *particle_pointer, dmin, davg, navg);
    }
}


void compute_forces_for_particle(grid &grid, size_t bin_id, particle_t &particle, double *dmin, double *davg, int *navg)
{
    particle.ax = particle.ay = 0;

    auto interacting_bins = acquire_interacting_bins(grid, bin_id);
    for (auto interacting_bin : interacting_bins) {
        particle_bin & bin = grid[interacting_bin];
        for (auto particle_pointer : bin) {
            particle_t & interacting_particle = *particle_pointer;

            // Exclude self-interaction
            if (particle_pointer != &particle) {
                // NOTE: Replace dmin, dmax etc. with real variables here asap
                apply_force(particle, interacting_particle, dmin, davg, navg);
            }
        }
    }
}


void compute_forces_for_grid(grid &grid, double *dmin, double *davg, int *navg)
{
    for (size_t i = 0; i < grid.bincount(); ++i) {
        compute_forces_for_bin(grid, i, dmin, davg, navg);
    }
}


size_t determine_bin_for_particle(const grid &grid, const particle_t &t)
{
    assert(t.x >= 0);
    assert(t.x <= grid.size);
    assert(t.y >= 0);
    assert(t.y <= grid.size);

    // Place particle in bin determined by x, y coordinates
    int x = static_cast<int> (t.x / grid.binsize_per_dimension);
    int y = static_cast<int> (t.y / grid.binsize_per_dimension);

    return x + y * grid.bins_per_dimension;
}
