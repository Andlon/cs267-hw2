#include <spatial_partition_omp.h>
#include <omp.h>
#include <algorithm>

namespace {
static const size_t FORCE_CHUNK_SIZE = 125;
static const size_t MOVE_CHUNK_SIZE = 250;
}

void update_forces_omp(partitioned_storage &storage, const grid &grid, double *dmin, double *davg, int *navg)
{
    static const size_t chunk_size = FORCE_CHUNK_SIZE;
    const size_t particle_count = storage.particles.size();

    #pragma omp for
    for (size_t i = 0; i < particle_count; i += chunk_size)
    {
        double local_dmin = 1.0;
        double local_davg = 0.0;
        int local_navg = 0;

        for (size_t j = i; j < std::min(particle_count, i + chunk_size); ++j)
            compute_particle_forces(storage, grid, storage.particles[j], &local_dmin, &local_davg, &local_navg);

        #pragma omp critical (statistics_critical)
        {
            // Update statistics
            *dmin = std::min(*dmin, local_dmin);
            *davg += local_davg;
            *navg += local_navg;
        }
    }
}

void move_particles_omp(partitioned_storage &storage)
{
    #pragma omp for schedule(static, MOVE_CHUNK_SIZE)
    for (size_t i = 0; i < storage.particles.size(); ++i)
        move(storage.particles[i]);
}
