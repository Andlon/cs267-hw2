#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <grid.h>
#include <omp.h>
#include "common.h"
#include "spatial_partition_omp.h"
#include <mpi.h>

//
//  benchmarking program
//
int main( int argc, char **argv )
{    
    int navg, nabsavg=0;
    double dmin, absmin=1.0,davg,absavg=0.0;
    double rdavg,rdmin;
    double local_absmin = absmin;
    int rnavg;

    //
    //  process command line parameters
    //
    if( find_option( argc, argv, "-h" ) >= 0 )
    {
        printf( "Options:\n" );
        printf( "-h to see this help\n" );
        printf( "-n <int> to set the number of particles\n" );
        printf( "-o <filename> to specify the output file name\n" );
        printf( "-s <filename> to specify a summary file name\n" );
        printf( "-no turns off all correctness checks and particle output\n");
        return 0;
    }
    
    int n = read_int( argc, argv, "-n", 1000 );
    char *savename = read_string( argc, argv, "-o", NULL );
    char *sumname = read_string( argc, argv, "-s", NULL );
    
    //
    //  set up MPI
    //
    int n_proc, rank;
    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &n_proc );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    
    //
    //  allocate generic resources
    //
    FILE *fsave = savename && rank == 0 ? fopen( savename, "w" ) : NULL;
    FILE *fsum = sumname && rank == 0 ? fopen ( sumname, "a" ) : NULL;

    // Create basic particle type
    static particle_t prototype;

    MPI_Aint prototype_address, x_address, y_address, vx_address, vy_address,
            ax_address, ay_address, partition_address;
    MPI_Aint x_offset, y_offset, vx_offset, vy_offset, ax_offset, ay_offset, partition_offset;

    MPI_Get_address(&prototype, &prototype_address);
    MPI_Get_address(&prototype.x, &x_address);
    MPI_Get_address(&prototype.y, &y_address);
    MPI_Get_address(&prototype.vx, &vx_address);
    MPI_Get_address(&prototype.vy, &vy_address);
    MPI_Get_address(&prototype.ax, &ax_address);
    MPI_Get_address(&prototype.ay, &ay_address);
    MPI_Get_address(&prototype.partition, &partition_address);

    x_offset = x_address - prototype_address;
    y_offset = y_address - prototype_address;
    vx_offset = vx_address - prototype_address;
    vy_offset = vy_address - prototype_address;
    ax_offset = ax_address - prototype_address;
    ay_offset = ay_address - prototype_address;
    partition_offset = partition_address - prototype_address;

    MPI_Datatype PARTICLE;
    int block_sizes[] = { 1, 1, 1, 1, 1, 1, 1 };
    MPI_Aint displacements[] = { x_offset, y_offset, vx_offset, vy_offset,
                                 ax_offset, ay_offset, partition_offset };
    MPI_Datatype types[] = { MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE,
                             MPI_DOUBLE, MPI_DOUBLE, MPI_UNSIGNED };
    MPI_Type_create_struct(7, block_sizes, displacements, types, &PARTICLE);
    MPI_Type_commit( &PARTICLE );

    // Set global size of domain
    set_size( n );

    // Initialize particle grid
    grid particle_grid(n, get_size(), get_cutoff());
    partitioned_storage storage(particle_grid);
    
    //  set up the data partitioning across processors
    int particle_per_proc = (n + n_proc - 1) / n_proc;
    int *partition_offsets = (int*) malloc( (n_proc+1) * sizeof(int) );
    for( int i = 0; i < n_proc+1; i++ )
        partition_offsets[i] = min( i * particle_per_proc, n );
    
    int *partition_sizes = (int*) malloc( n_proc * sizeof(int) );
    for( int i = 0; i < n_proc; i++ )
        partition_sizes[i] = partition_offsets[i+1] - partition_offsets[i];
    
    //  allocate storage for local partition
    int nlocal = partition_sizes[rank];
    std::vector<particle_t> local(nlocal);
    
    //  initialize and distribute the particles (that's fine to leave it unoptimized)
    if( rank == 0 )
        init_particles( n, &storage.particles[0]);
    MPI_Scatterv( &storage.particles[0], partition_sizes, partition_offsets, PARTICLE, &local[0], nlocal, PARTICLE, 0, MPI_COMM_WORLD );
    
    //  simulate a number of time steps
    double simulation_time = read_timer( );
    for( int step = 0; step < NSTEPS; step++ )
    {
        navg = 0;
        dmin = 1.0;
        davg = 0.0;

        //  collect all global data locally (not good idea to do)
        MPI_Allgatherv( &local[0], nlocal, PARTICLE, &storage.particles[0], partition_sizes, partition_offsets, PARTICLE, MPI_COMM_WORLD );
        
        //  save current step if necessary (slightly different semantics than in other codes)
        if( find_option( argc, argv, "-no" ) == -1 )
            if( fsave && (step%SAVEFREQ) == 0 )
                save( fsave, n, &storage.particles[0] );

        // Update particles
        update_partitions(local, particle_grid);
//        parallel_partition(storage, particle_grid);
//        #pragma omp parallel
//        {
//            update_forces_omp(local, storage, particle_grid, &dmin, &davg, &navg);
//            move_particles_omp(local);
//        }

        // Temporarily use single thread
        partition(storage, particle_grid);
        update_forces(local, storage, particle_grid, &dmin, &davg, &navg);
        move_particles(local);

        if( find_option( argc, argv, "-no" ) == -1 )
        {
            MPI_Reduce(&davg,&rdavg,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
            MPI_Reduce(&navg,&rnavg,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
            MPI_Reduce(&dmin,&rdmin,1,MPI_DOUBLE,MPI_MIN,0,MPI_COMM_WORLD);

            if (rank == 0)
            {
                // Computing statistical data
                if (rnavg)
                {
                    absavg +=  rdavg/rnavg;
                    nabsavg++;
                }
                if (rdmin < absmin)
                    absmin = rdmin;
            }
        }
    }
    simulation_time = read_timer( ) - simulation_time;

    if (rank == 0) {
        printf( "n = %d, simulation time = %g seconds", n, simulation_time);

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
            fprintf(fsum,"%d %d %g\n",n,n_proc,simulation_time);
    }

    //
    //  release resources
    //
    if ( fsum )
        fclose( fsum );
    free( partition_offsets );
    free( partition_sizes );
    if( fsave )
        fclose( fsave );
    
    MPI_Finalize( );
    
    return 0;
}
