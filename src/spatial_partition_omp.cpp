#include <spatial_partition_omp.h>

void update_forces_omp(partitioned_storage &storage, const grid &grid, double *dmin, double *davg, int *navg)
{
    #pragma omp for reduction (+:*navg) reduction(+:*davg)
    for (size_t i = 0; i < storage.particles.size(); ++i)
    {
        compute_particle_forces(storage, grid, storage.particles[i], dmin, davg, navg);
    }
}

void move_particles_omp(partitioned_storage &storage)
{
    #pragma omp for
    for (size_t i = 0; i < storage.particles.size(); ++i)
        move(storage.particles[i]);
}
