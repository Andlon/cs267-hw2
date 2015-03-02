#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "util/common.h"
#include "omp.h"

//
//  benchmarking program
//
int main( int argc, char **argv )
{   
  int navg,nabsavg=0,numthreads; 
  double dmin, absmin=1.0,davg,absavg=0.0;
  
  if( find_option( argc, argv, "-h" ) >= 0 )
  {
    printf( "Options:\n" );
    printf( "-h to see this help\n" );
    printf( "-n <int> to set number of particles\n" );
    printf( "-o <filename> to specify the output file name\n" );
    printf( "-s <filename> to specify a summary file name\n" );
    printf( "-p <int> to set number of threads to use\n");
    printf( "-no turns off all correctness checks and particle output\n");   
    return 0;
  }

  int n = read_int( argc, argv, "-n", 1000 );
  char *savename = read_string( argc, argv, "-o", NULL );
  char *sumname = read_string( argc, argv, "-s", NULL );

  FILE *fsave = savename ? fopen( savename, "w" ) : NULL;
  FILE *fsum = sumname ? fopen ( sumname, "a" ) : NULL;      

  int p = read_int( argc, argv, "-p", 1 );
  omp_set_num_threads(p);

  particle_t *particles = (particle_t*) malloc( n * sizeof(particle_t) );
  set_size( n );
  init_particles( n, particles );

  bin_t *bins;
  int n_bins = init_bins(bins);
  int n_bins_per_side = sqrt(n_bins);
  omp_lock_t* locks = (omp_lock_t*) malloc(n_bins * sizeof(omp_lock_t) );
  for (int i = 0; i < n_bins; i++) {
    omp_init_lock(locks+i);
  }

  //
  //  simulate a number of time steps
  //
  double simulation_time = read_timer( );


  #pragma omp parallel private(dmin)
  {
    numthreads = omp_get_num_threads();

    for( int step = 0; step < NSTEPS; step++ )
    {
      navg = 0;
      davg = 0.0;
      dmin = 1.0;

      //  assign particles to bins
      #pragma omp for
      for (int i = 0; i < n_bins_per_side; i++) {
        for (int j = 0; j < n_bins_per_side; j++) {
          bins[i*n_bins_per_side+j].clear();
        }
      }

      #pragma omp for 
      for (int i = 0; i < n; i++) {
        int bin_x_id = floor(particles[i].x / bin_size);
        int bin_y_id = floor(particles[i].y / bin_size);
        int bin_id = bin_x_id * n_bins_per_side + bin_y_id;
        omp_set_lock(locks+bin_id);
        bins[bin_id].add_particle(particles+i);
        omp_unset_lock(locks+bin_id);
      }

//      #pragma omp single
//      bin_particles(bins, particles, n);
 
      //
      //  compute all forces
      //
      if( find_option( argc, argv, "-no" ) == -1 ) {
        #pragma omp for reduction (+:navg) reduction(+:davg)
        for (int i = 0; i < n_bins; i++) {
          apply_force_bin (bins, i, &dmin,davg,navg);
        }
      } else {
        #pragma omp for
        for (int i = 0; i < n_bins; i++) {
          apply_force_bin (bins, i, NULL, davg, navg);
        }
      }

      //
      //  move particles
      //
      #pragma omp for
      for( int i = 0; i < n; i++ ) 
        move( particles[i] );

      if( find_option( argc, argv, "-no" ) == -1 ) 
      {
        //
        //  compute statistical data
        //
        #pragma omp single
        if (navg) { 
          absavg += davg/navg;
          nabsavg++;
        }

        #pragma omp critical
        if (dmin < absmin) absmin = dmin; 
      
        //
        //  save if necessary
        //
        #pragma omp single
        if( fsave && (step%SAVEFREQ) == 0 )
          save( fsave, n, particles );
      }
    }
  }

  simulation_time = read_timer( ) - simulation_time;

  printf( "n = %d,threads = %d, simulation time = %g seconds", n,numthreads, simulation_time);

  if( find_option( argc, argv, "-no" ) == -1 )
  {
    if (nabsavg) absavg /= nabsavg;
    // 
    //  -the minimum distance absmin between 2 particles during the run of the simulation
    //  -A Correct simulation will have particles stay at greater than 0.4 (of cutoff) with typical values between .7-.8
    //  -A simulation were particles don't interact correctly will be less than 0.4 (of cutoff) with typical values between .01-.05
    //
    //  -The average distance absavg is ~.95 when most particles are interacting correctly and ~.66 when no particles are interacting
    //
    printf( ", absmin = %lf, absavg = %lf", absmin, absavg);
    if (absmin < 0.4) printf ("\nThe minimum distance is below 0.4 meaning that some particle is not interacting");
    if (absavg < 0.8) printf ("\nThe average distance is below 0.8 meaning that most particles are not interacting");
  }
  printf("\n");

  //
  // Printing summary data
  //
  if( fsum)
    fprintf(fsum,"%d %d %g\n",n,numthreads,simulation_time);

  //
  // Clearing space
  //
  if( fsum )
    fclose( fsum );

  clear_bins(bins);
  free( particles );
  if( fsave )
    fclose( fsave );

  return 0;
}
