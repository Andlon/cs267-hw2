#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cmath>
#include "omp.h"
#include "../util/spatial_partition_omp.h"

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
        printf( "-no turns off all correctness checks and particle output\n");
        return 0;
    }

    int n = read_int( argc, argv, "-n", 1000 );
    char *savename = read_string( argc, argv, "-o", NULL );
    char *sumname = read_string( argc, argv, "-s", NULL );

    FILE *fsave = savename ? fopen( savename, "w" ) : NULL;
    FILE *fsum = sumname ? fopen ( sumname, "a" ) : NULL;

    set_size( n );
    grid particle_grid(n, get_size(), get_cutoff());
    partitioned_storage storage(particle_grid);
    init_particles( n, &storage.particles[0] );

    //  simulate a number of time steps
    double simulation_time = read_timer( );

    for( int step = 0; step < 1000; step++ )
    {
        // Note that parallel_partition must be called outside
        // the parallel region because it is not thread-safe.
        // Rather, it spawns internal threads indirectly through GCC's
        // parallel algorithm (don't think it's possible to reuse threadpool
        // from parallel region).
        parallel_partition(storage, particle_grid);

        navg = 0;
        davg = 0.0;

        #pragma omp parallel private(dmin)
        {
            numthreads = omp_get_num_threads();
            dmin = 1.0;

            update_forces_omp(storage, particle_grid, &dmin, &davg, &navg);
            move_particles_omp(storage);

            if( find_option( argc, argv, "-no" ) == -1 )
            {
                //  compute statistical data
                #pragma omp master
                if (navg) {
                    absavg += davg/navg;
                    nabsavg++;
                }

                #pragma omp critical
                if (dmin < absmin) absmin = dmin;

                //  save if necessary
                #pragma omp master
                if( fsave && (step%SAVEFREQ) == 0 )
                    save( fsave, n, &storage.particles[0] );
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

    if( fsave )
        fclose( fsave );
    
    return 0;
}
