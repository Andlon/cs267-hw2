#include <grid.h>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <iterator>

namespace {

int compute_bins_per_dimension(double size, double minimum_bin_size) {
    assert(size > 0);
    assert(minimum_bin_size > 0);
    minimum_bin_size = std::min(size, minimum_bin_size);
    return static_cast<int> (size / minimum_bin_size);
}

}

std::vector<size_t> acquire_interacting_bins(const grid & grid, size_t bin_id) {
    assert(bin_id < grid.bincount());

    int x0 = ((int) bin_id) % grid.bins_per_dim();
    int y0 = ((int) bin_id) / grid.bins_per_dim();

    // All coordinates of neighboring bins, including this bin
    std::pair<int, int> bins_xy[] = {
        { x0 - 1, y0 - 1 },
        { x0,     y0 - 1 },
        { x0 + 1, y0 - 1 },
        { x0 - 1, y0     },
        { x0,     y0     },
        { x0 + 1, y0     },
        { x0 - 1, y0 + 1 },
        { x0,     y0 + 1 },
        { x0 + 1, y0 + 1 }
    };

    // Transform coordinates to indices, filtering out non-existent bins
    std::vector<size_t> bin_ids;
    for (const auto & coord : bins_xy)
    {
        int x = coord.first;
        int y = coord.second;
        if (    in_range(x, 0, grid.bins_per_dim()) &&
                in_range(y, 0, grid.bins_per_dim()))
        {
            size_t id = static_cast<size_t> (coord.first + coord.second * grid.bins_per_dim());
            bin_ids.push_back(id);
        }
    }

    return bin_ids;
}

void assign_neighbors_to_bins(const grid & grid, std::vector<particle_bin> & bins) {
    for (size_t i = 0; i < bins.size(); ++i)
        bins[i].neighbors = acquire_interacting_bins(grid, i);
}

grid::grid(double gridsize, double minimum_bin_size)
    : _bins_per_dim(compute_bins_per_dimension(gridsize, minimum_bin_size)),
      _size(gridsize)
{
    _binsize = _size / _bins_per_dim;

    _bins.resize(bincount());
    assign_neighbors_to_bins(*this, _bins);
}

size_t grid::bincount() const
{
    return bins_per_dim() * bins_per_dim();
}

size_t grid::bins_per_dim() const
{
    return _bins_per_dim;
}

double grid::binsize() const
{
    return _binsize;
}

double grid::size() const
{
    return _size;
}

particle_bin &grid::operator[](size_t bin_id) {
    assert(bin_id < _bins.size());
    return _bins[bin_id];
}

const particle_bin &grid::operator[](size_t bin_id) const
{
    return const_cast<const particle_bin &>((*this)[bin_id]);
}

void grid::clear_particles()
{
    for (auto & bin : _bins)
        bin.particles.clear();
}

void grid::distribute_particles(particle_t *particles, size_t count)
{
    for (size_t i = 0; i < count; ++i) {
        particle_t * particle = particles + i;
        size_t bin = determine_bin_for_particle(*this, *particle);
        _bins[bin].particles.push_back(particle);
    }
}

void compute_forces_for_bin(grid &grid, size_t bin_id, double *dmin, double *davg, int *navg)
{
    assert(in_range(bin_id, 0, grid.bincount()));
    for (auto particle_pointer : grid[bin_id].particles) {
        compute_forces_for_particle(grid, bin_id, *particle_pointer, dmin, davg, navg);
    }
}


void compute_forces_for_particle(grid &grid, size_t bin_id, particle_t &particle, double *dmin, double *davg, int *navg)
{
    particle.ax = particle.ay = 0;

    auto & bin = grid[bin_id];
    for (auto interacting_bin : bin.neighbors) {
        particle_bin & bin = grid[interacting_bin];
        for (auto particle_pointer : bin.particles) {
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
    assert(t.x <= grid.size());
    assert(t.y >= 0);
    assert(t.y <= grid.size());

    // Place particle in bin determined by x, y coordinates
    int x = static_cast<int> (t.x / grid.binsize());
    int y = static_cast<int> (t.y / grid.binsize());

    return static_cast<size_t>(x + y * grid.bins_per_dim());
}
