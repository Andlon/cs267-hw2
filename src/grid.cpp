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

std::vector<size_t> acquire_interacting_bins(const grid & grid, size_t bin_id) {
    assert(bin_id < grid.bincount());

    int x0 = ((int) bin_id % grid.bins_per_dimension);
    int y0 = ((int) bin_id / grid.bins_per_dimension);

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
    std::vector<size_t> bin_ids(9);
    for (const auto & coord : bins_xy)
    {
        int x = coord.first;
        int y = coord.second;
        if (    in_range(x, 0, grid.binsize_per_dimension) &&
                in_range(y, 0, grid.binsize_per_dimension))
        {
            size_t id = static_cast<size_t> (coord.first + coord.second * grid.binsize_per_dimension);
            bin_ids.push_back(id);
        }
    }

    return bin_ids;
}

}

// Note:
// avoid using bins_per_dimension in the definition of
// binsize_per_dimension, as if the definition in the header file
// is accidentally reordered, it may introduce an insidious bug. Hence,
// we use the computeNumberOfBins function to compute every time.
grid::grid(double size, double minimum_bin_size)
    : bins_per_dimension(computeNumberOfBins(size, minimum_bin_size)),
      binsize_per_dimension( size / computeNumberOfBins(size, minimum_bin_size) ),
      size(size)
{
    bins.resize(bincount());
}

size_t grid::bincount() const
{
    return binsize_per_dimension * binsize_per_dimension;
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
        size_t bin = compute_bin_for_particle(*this, *particle);
        bins[bin].push_back(particle);
    }
}


void calculate_forces_for_bin(grid &grid, size_t bin_id)
{
    assert(in_range(bin_id, 0, grid.bincount()));
    for (auto particle_pointer : grid[bin_id]) {
        calculate_forces_for_particle(grid, bin_id, *particle_pointer);
    }
}


void calculate_forces_for_particle(grid &grid, size_t bin_id, particle_t &particle)
{
    auto interacting_bins = acquire_interacting_bins(grid, bin_id);
    for (auto interacting_bin : interacting_bins) {
        particle_bin & bin = grid[interacting_bin];
        for (auto particle_pointer : bin) {
            particle_t & interacting_particle = *particle_pointer;

            // Exclude self-interaction
            if (particle_pointer != &particle) {
                // NOTE: Replace dmin, dmax etc. with real variables here asap
                apply_force(particle, interacting_particle, 0, 0, 0);
            }
        }
    }
}


void calculate_forces_for_grid(grid &grid)
{
    for (size_t i = 0; i < grid.bincount(); ++i) {
        calculate_forces_for_bin(grid, i);
    }
}
