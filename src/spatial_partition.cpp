#include <spatial_partition.h>
#include <algorithm>
#include <cassert>


void partition(partitioned_storage &storage, const grid &grid)
{
    int test = storage.particles.size();
    // Sort all the particles by their partition indices
    std::sort(storage.particles.begin(), storage.particles.end(),
              [&grid] (const particle_t & a, const particle_t & b)
    {
        return determine_bin_for_particle(grid, a) < determine_bin_for_particle(grid, b);
    });

    // After sorting, figure out where the different partitions begin and end
    size_t partition = 0;
    for (size_t i = 0; i < storage.particles.size(); ++i)
    {
        const particle_t & particle = storage.particles[i];
        size_t current_particle_partition = determine_bin_for_particle(grid, particle);

        while (current_particle_partition > partition) {
            storage.partitions[partition] = i;
            ++partition;
        }
    }

    // Last partition always spans the last part of the vector
    storage.partitions.back() = storage.particles.size();
}


partitioned_storage::partitioned_storage(const grid &particle_grid)
    : particles(particle_grid.particle_count()),
      partitions(particle_grid.bincount())
{

}
