#include "spatial_partition.h"
#include <algorithm>
#include <cassert>

namespace {

struct location {
    int x;
    int y;
};

static const location NEIGHBORHOOD[] = {
    { -1, -1 },
    {  0, -1 },
    {  1, -1 },
    { -1,  0 },
    {  0,  0 },
    {  1,  0 },
    { -1,  1 },
    {  0,  1 },
    {  1,  1 }
};

// Consider a particle to be equal if it occupies exactly the same location
bool particles_equal(const particle_t & a, const particle_t & b) {
    return  a.partition == b.partition &&
            a.x == b.x &&
            a.y == b.y;
}

}

partitioned_storage::partitioned_storage(const grid &particle_grid)
    : particles(particle_grid.particle_count()),
      partitions(particle_grid.partition_count())
{

}

void partition(partitioned_storage &storage, const grid &grid)
{
    assert(storage.particles.size() > 0);
    assert(storage.partitions.size() > 0);

    update_partitions(storage.particles, grid);

    // Sort all the particles by their partition indices
    std::sort(storage.particles.begin(), storage.particles.end(),
              [&grid] (const particle_t & a, const particle_t & b)
    {
        return a.partition < b.partition;
    });

    // After sorting, figure out where the different partitions begin and end
    size_t partition = 0;
    for (size_t i = 0; i < storage.particles.size(); ++i)
    {
        const particle_t & particle = storage.particles[i];
        while (particle.partition > partition) {
            storage.partitions[partition] = i;
            ++partition;
        }
    }

    // The partition associated with the last particle
    // spans the rest of the particle vector
    for (size_t i = partition; i < storage.partitions.size(); ++i)
        storage.partitions[i] = storage.particles.size();
}

void update_forces(std::vector<particle_t> &particles, partitioned_storage &storage, const grid &grid, double *dmin, double *davg, int *navg)
{
    for (auto & particle : particles)
    {
        compute_particle_forces(storage, grid, particle, dmin, davg, navg);
    }
}

void update_forces(partitioned_storage &storage, const grid &grid, double *dmin, double *davg, int *navg)
{
    update_forces(storage.particles, storage, grid, dmin, davg, navg);
}

void compute_particle_forces(partitioned_storage &storage, const grid &grid, particle_t & particle, double *dmin, double *davg, int *navg)
{
    particle.ax = particle.ay = 0;

    int x0 = ((int) particle.partition) % grid.partitions_per_dim();
    int y0 = ((int) particle.partition) / grid.partitions_per_dim();

    // Skip validity checks on neighborhood for interior partitions
    if (grid.partitions_per_dim() > 2 &&
            in_range(x0, 1, grid.partitions_per_dim() - 1) &&
            in_range(y0, 1, grid.partitions_per_dim() - 1))
    {
        // Iterate over neighborhood (includes partition containing particle)
        for (const auto & location : NEIGHBORHOOD)
        {
            int x = x0 + location.x;
            int y = y0 + location.y;

            // Interact with particles in partition
            size_t partition_index = static_cast<size_t> (x + y * grid.partitions_per_dim());
            compute_particle_forces_in_partition(storage, particle, partition_index,
                                                 dmin, davg, navg);
        }
    }
    else
    {
        // Iterate over neighborhood (includes partition containing particle)
        for (const auto & location : NEIGHBORHOOD)
        {
            int x = x0 + location.x;
            int y = y0 + location.y;

            // Check for validity in case of boundaries
            if (    in_range(x, 0, grid.partitions_per_dim()) &&
                    in_range(y, 0, grid.partitions_per_dim()))
            {
                // Partition is valid. Interact with particles in partition
                size_t partition_index = static_cast<size_t> (x + y * grid.partitions_per_dim());
                compute_particle_forces_in_partition(storage, particle, partition_index,
                                                     dmin, davg, navg);
            }
        }
    }
}

void compute_particle_forces_in_partition(partitioned_storage &storage, particle_t &particle, size_t partition_index, double *dmin, double *davg, int *navg)
{
    size_t start = partition_index == 0 ? 0 : storage.partitions[partition_index - 1];
    size_t end = storage.partitions[partition_index];

    // Iterate over particles in partition [start, end)
    for (size_t i = start; i < end; ++i) {
        const particle_t & neighbor = storage.particles[i];
        if (!particles_equal(particle, neighbor)) {
            apply_force(particle, neighbor, dmin, davg, navg);
        }
    }
}


void move_particles(partitioned_storage &storage)
{
    move_particles(storage.particles);
}


void move_particles(std::vector<particle_t> &particles)
{
    for (auto & particle : particles)
        move(particle);
}


void update_partitions(std::vector<particle_t> &particles, const grid &grid)
{
    for (auto & particle : particles) {
        particle.partition = determine_partition_for_particle(grid, particle);
        assert(particle.partition < grid.partition_count());
    }
}
