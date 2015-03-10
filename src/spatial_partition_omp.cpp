#include <spatial_partition_omp.h>
#include <omp.h>
#include <algorithm>
#include <cassert>
#include <parallel/algorithm>

namespace {
static const size_t FORCE_CHUNK_SIZE = 125;
static const size_t MOVE_CHUNK_SIZE = 250;
}

void update_forces_omp(std::vector<particle_t> & particles, partitioned_storage &storage,
                       const grid &grid, double *dmin, double *davg, int *navg)
{
    static const size_t chunk_size = FORCE_CHUNK_SIZE;
    const size_t particle_count = particles.size();

    #pragma omp for
    for (size_t i = 0; i < particle_count; i += chunk_size)
    {
        double local_dmin = 1.0;
        double local_davg = 0.0;
        int local_navg = 0;

        for (size_t j = i; j < std::min(particle_count, i + chunk_size); ++j)
            compute_particle_forces(storage, grid, particles[j], &local_dmin, &local_davg, &local_navg);

        #pragma omp critical (statistics_critical)
        {
            // Update statistics
            *dmin = std::min(*dmin, local_dmin);
            *davg += local_davg;
            *navg += local_navg;
        }
    }
}

void update_forces_omp(partitioned_storage &storage, const grid &grid, double *dmin, double *davg, int *navg)
{
    update_forces_omp(storage.particles, storage, grid, dmin, davg, navg);
}

void move_particles_omp(std::vector<particle_t> &particles)
{
    #pragma omp for schedule(static, MOVE_CHUNK_SIZE)
    for (size_t i = 0; i < particles.size(); ++i)
        move(particles[i]);
}

void move_particles_omp(partitioned_storage &storage)
{
    move_particles_omp(storage.particles);
}


// Note: this is pretty much a copy-paste from the sequential
// version. The only thing edited is the use of parallel sort.
void parallel_partition(partitioned_storage &storage, const grid &grid)
{
    assert(storage.particles.size() > 0);
    assert(storage.partitions.size() > 0);

    for (auto & particle : storage.particles)
        particle.partition = determine_bin_for_particle(grid, particle);

    // Sort all the particles by their partition indices
    __gnu_parallel::sort(storage.particles.begin(), storage.particles.end(),
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
