#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <cuda.h>
#include "util/common.h"

#define NUM_THREADS 256

extern double size;
//
//  benchmarking program
//

__device__ void apply_force_gpu(particle_t &particle, particle_t &neighbor)
{
  double dx = neighbor.x - particle.x;
  double dy = neighbor.y - particle.y;
  double r2 = dx * dx + dy * dy;
  if( r2 > cutoff*cutoff )
      return;
  //r2 = fmax( r2, min_r*min_r );
  r2 = (r2 > min_r*min_r) ? r2 : min_r*min_r;
  double r = sqrt( r2 );

  //
  //  very simple short-range repulsive force
  //
  double coef = ( 1 - cutoff / r ) / r2 / mass;
  particle.ax += coef * dx;
  particle.ay += coef * dy;

}

__device__ void apply_force_particle_bin(particle_t &particle, int bin_id, particle_t **bins,
                                         int* num_particles_per_bin, int n_bins_per_side, int max_particles_per_bin)
{
  int bin_x_id = bin_id / n_bins_per_side;
  int bin_y_id = bin_id % n_bins_per_side;

  // reset acceleration
  particle.ax = particle.ay = 0;

  // apply force within bin
  for (int j = 0; j < num_particles_per_bin[bin_id]; j++) {
    apply_force_gpu(particle, *(bins[bin_id*max_particles_per_bin+j]));
  }

  // apply force from edge-neighboring bins
  if (bin_x_id != 0) {
    int neighbor_id = bin_id - n_bins_per_side;
    for (int j = 0; j < num_particles_per_bin[neighbor_id]; j++) {
      apply_force_gpu(particle, *(bins[neighbor_id*max_particles_per_bin+j]));
    }
  }
  if (bin_x_id != n_bins_per_side-1) {
    int neighbor_id = bin_id + n_bins_per_side;
    for (int j = 0; j < num_particles_per_bin[neighbor_id]; j++) {
      apply_force_gpu(particle, *(bins[neighbor_id*max_particles_per_bin+j]));
    }
  }
  if (bin_y_id != 0) {
    int neighbor_id = bin_id - 1;
    for (int j = 0; j < num_particles_per_bin[neighbor_id]; j++) {
      apply_force_gpu(particle, *(bins[neighbor_id*max_particles_per_bin+j]));
    }
  }
  if (bin_y_id != n_bins_per_side-1) {
    int neighbor_id = bin_id + 1;
    for (int j = 0; j < num_particles_per_bin[neighbor_id]; j++) {
      apply_force_gpu(particle, *(bins[neighbor_id*max_particles_per_bin+j]));
    }
  }

  // apply force from edge-neighboring bins
  if (bin_x_id != 0 && bin_y_id != 0) {
    int neighbor_id = bin_id - (n_bins_per_side + 1);
    for (int j = 0; j < num_particles_per_bin[neighbor_id]; j++) {
      apply_force_gpu(particle, *(bins[neighbor_id*max_particles_per_bin+j]));
    }
  }
  if (bin_x_id != 0 && bin_y_id != n_bins_per_side-1) {
    int neighbor_id = bin_id - (n_bins_per_side - 1);
    for (int j = 0; j < num_particles_per_bin[neighbor_id]; j++) {
      apply_force_gpu(particle, *(bins[neighbor_id*max_particles_per_bin+j]));
    }
  }
  if (bin_x_id != n_bins_per_side-1 && bin_y_id != 0) {
    int neighbor_id = bin_id + n_bins_per_side - 1;
    for (int j = 0; j < num_particles_per_bin[neighbor_id]; j++) {
      apply_force_gpu(particle, *(bins[neighbor_id*max_particles_per_bin+j]));
    }
  }
  if (bin_x_id != n_bins_per_side-1 && bin_y_id != n_bins_per_side-1) {
    int neighbor_id = bin_id + n_bins_per_side + 1;
    for (int j = 0; j < num_particles_per_bin[neighbor_id]; j++) {
      apply_force_gpu(particle, *(bins[neighbor_id*max_particles_per_bin+j]));
    }
  }
}

__global__ void bin_particles_gpu(particle_t **bins, int* num_particles_per_bin, int n_bins_per_side, 
                                  int max_particles_per_bin, particle_t *particles, int n) 
{
  int tid = threadIdx.x + blockIdx.x * blockDim.x;

  if (tid >= n) return;

  int bin_x_id = floor(particles[tid].x / bin_size);
  int bin_y_id = floor(particles[tid].y / bin_size);
  int bin_id = bin_x_id * n_bins_per_side + bin_y_id;
  int id_in_bin = atomicAdd(num_particles_per_bin+bin_id, 1);

  bins[bin_id * max_particles_per_bin + id_in_bin] = particles+tid;
}

__global__ void compute_forces_bin_gpu(particle_t **bins, int* num_particles_per_bin, int n_bins_per_side,
                                       int max_particles_per_bin)
{
  int tid = threadIdx.x + blockIdx.x * blockDim.x;
  int n_bins = n_bins_per_side * n_bins_per_side;
  if (tid >= n_bins) return;

  if (num_particles_per_bin[tid] == 0) return;

  for (int i = 0; i < num_particles_per_bin[tid]; i++) {
    apply_force_particle_bin(*(bins[tid*max_particles_per_bin+i]), tid, bins, num_particles_per_bin,
                             n_bins_per_side, max_particles_per_bin);
  }
}

__global__ void compute_forces_gpu(particle_t * particles, int n)
{
  // Get thread (particle) ID
  int tid = threadIdx.x + blockIdx.x * blockDim.x;
  if(tid >= n) return;

  particles[tid].ax = particles[tid].ay = 0;
  for(int j = 0 ; j < n ; j++)
    apply_force_gpu(particles[tid], particles[j]);

}

__global__ void move_gpu (particle_t * particles, int n, double size)
{

  // Get thread (particle) ID
  int tid = threadIdx.x + blockIdx.x * blockDim.x;
  if(tid >= n) return;

  particle_t * p = &particles[tid];
  //
  //  slightly simplified Velocity Verlet integration
  //  conserves energy better than explicit Euler method
  //
  p->vx += p->ax * dt;
  p->vy += p->ay * dt;
  p->x  += p->vx * dt;
  p->y  += p->vy * dt;

  //
  //  bounce from walls
  //
  while( p->x < 0 || p->x > size )
  {
      p->x  = p->x < 0 ? -(p->x) : 2*size-p->x;
      p->vx = -(p->vx);
  }
  while( p->y < 0 || p->y > size )
  {
      p->y  = p->y < 0 ? -(p->y) : 2*size-p->y;
      p->vy = -(p->vy);
  }

}



int main( int argc, char **argv )
{    
    // This takes a few seconds to initialize the runtime
    cudaThreadSynchronize(); 

    if( find_option( argc, argv, "-h" ) >= 0 )
    {
        printf( "Options:\n" );
        printf( "-h to see this help\n" );
        printf( "-n <int> to set the number of particles\n" );
        printf( "-o <filename> to specify the output file name\n" );
        return 0;
    }
    
    int n = read_int( argc, argv, "-n", 1000 );

    char *savename = read_string( argc, argv, "-o", NULL );
    
    FILE *fsave = savename ? fopen( savename, "w" ) : NULL;
    particle_t *particles = (particle_t*) malloc( n * sizeof(particle_t) );

    // GPU particle data structure
    particle_t * d_particles;
    cudaMalloc((void **) &d_particles, n * sizeof(particle_t));

    set_size( n );

    init_particles( n, particles );

    particle_t **bins;
    int n_bins_per_side = ceil(size / bin_size);
    int n_bins = n_bins_per_side * n_bins_per_side;
    int max_num_particles_per_bin_per_side = 8;
    int max_num_particles_per_bin = (ceil(bin_size/cutoff)*max_num_particles_per_bin_per_side) * (ceil(bin_size/cutoff)*max_num_particles_per_bin_per_side);
    cudaMalloc((void ***) &bins, max_num_particles_per_bin * n_bins * sizeof(particle_t *));

    int* num_particles_per_bin;
    cudaMalloc((void **) &num_particles_per_bin, n_bins * sizeof(int));

    cudaThreadSynchronize();
    double copy_time = read_timer( );

    // Copy the particles to the GPU
    cudaMemcpy(d_particles, particles, n * sizeof(particle_t), cudaMemcpyHostToDevice);

    cudaThreadSynchronize();
    copy_time = read_timer( ) - copy_time;
    
    //
    //  simulate a number of time steps
    //
    cudaThreadSynchronize();
    double simulation_time = read_timer( );

    int blks = (n + NUM_THREADS - 1) / NUM_THREADS;
    int bin_blks = (n_bins + NUM_THREADS - 1) / NUM_THREADS;

    for( int step = 0; step < NSTEPS; step++ )
    {
      //
      //  compute forces
      //
      cudaMemset(num_particles_per_bin, 0, n_bins * sizeof(int));

      bin_particles_gpu <<<blks, NUM_THREADS>>> (bins, num_particles_per_bin, n_bins_per_side, 
                                                 max_num_particles_per_bin, d_particles, n);

      compute_forces_bin_gpu <<<bin_blks, NUM_THREADS>>> (bins, num_particles_per_bin, n_bins_per_side,
                                                          max_num_particles_per_bin);
//      compute_forces_gpu <<< blks, NUM_THREADS >>> (d_particles, n);
        
      //
      //  move particles
      //
      move_gpu <<< blks, NUM_THREADS >>> (d_particles, n, size);
      
      //
      //  save if necessary
      //
      if( fsave && (step%SAVEFREQ) == 0 ) {
        // Copy the particles back to the CPU
        cudaMemcpy(particles, d_particles, n * sizeof(particle_t), cudaMemcpyDeviceToHost);
        save( fsave, n, particles);
      }
    }
    cudaThreadSynchronize();
    simulation_time = read_timer( ) - simulation_time;
    
    printf( "CPU-GPU copy time = %g seconds\n", copy_time);
    printf( "n = %d, simulation time = %g seconds\n", n, simulation_time );
    
    free( particles );
    cudaFree(d_particles);
    if( fsave )
        fclose( fsave );
    
    return 0;
}
