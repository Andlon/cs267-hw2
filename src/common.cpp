#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <float.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "common.h"

double size;
int n_bins_per_side;

//
//  timer
//
double read_timer( )
{
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

//
//  keep density constant
//
void set_size( int n )
{
    size = sqrt( density * n );
}

//
//  Initialize the particle positions and velocities
//
void init_particles( int n, particle_t *p )
{
//  we force same randomness for now
//    srand48( time( NULL ) );
    srand48(0);
        
    int sx = (int)ceil(sqrt((double)n));
    int sy = (n+sx-1)/sx;
    
    int *shuffle = (int*)malloc( n * sizeof(int) );
    for( int i = 0; i < n; i++ )
        shuffle[i] = i;
    
    for( int i = 0; i < n; i++ ) 
    {
        //
        //  make sure particles are not spatially sorted
        //
        int j = lrand48()%(n-i);
        int k = shuffle[j];
        shuffle[j] = shuffle[n-i-1];
        
        //
        //  distribute particles evenly to ensure proper spacing
        //
        p[i].x = size*(1.+(k%sx))/(1+sx);
        p[i].y = size*(1.+(k/sx))/(1+sy);

        //
        //  assign random velocities within a bound
        //
        p[i].vx = drand48()*2-1;
        p[i].vy = drand48()*2-1;
    }
    free( shuffle );
}

//
// Allocate bins
//
int init_bins (bin_t*& bins) {
  n_bins_per_side = ceil(size / bin_size);
  int n_bins = n_bins_per_side * n_bins_per_side;
  bins = new bin_t[n_bins];
  return n_bins;
}

void clear_bins(bin_t*& bins) {
  delete [] bins;
}

//
// Assign particles to bins
//
void bin_particles( bin_t *bins, particle_t *particles, int n) {
  for (int i = 0; i < n_bins_per_side; i++) {
    for (int j = 0; j < n_bins_per_side; j++) {
      bins[i*n_bins_per_side+j].clear();
    }
  }
  for (int i = 0; i < n; i++) {
    int bin_x_id = floor(particles[i].x / bin_size);
    int bin_y_id = floor(particles[i].y / bin_size);
    int bin_id = bin_x_id * n_bins_per_side + bin_y_id;
    bins[bin_id].add_particle(particles+i);
  }
}


//
//  interact two particles
//
void apply_force( particle_t &particle, particle_t &neighbor , double *dmin, double &davg, int &navg)
{

    double dx = neighbor.x - particle.x;
    double dy = neighbor.y - particle.y;
    double r2 = dx * dx + dy * dy;
    if( r2 > cutoff*cutoff )
        return;
    if (dmin != NULL && r2 != 0)
    {
       if (r2/(cutoff*cutoff) < *dmin * (*dmin))
          *dmin = sqrt(r2)/cutoff;
       davg += sqrt(r2)/cutoff;
       navg ++;
    }
        
    r2 = fmax( r2, min_r*min_r );
    double r = sqrt( r2 );
    
    //
    //  very simple short-range repulsive force
    //
    double coef = ( 1 - cutoff / r ) / r2 / mass;
    particle.ax += coef * dx;
    particle.ay += coef * dy;
}

void apply_force_bin( bin_t *bins, int bin_id, double *dmin, double &davg, int &navg) {
  if (bins[bin_id].num_particles() == 0) // nothing to apply
    return;

  int bin_x_id = bin_id / n_bins_per_side;
  int bin_y_id = bin_id % n_bins_per_side;

  for (int i = 0; i < bins[bin_id].num_particles(); i++) {
    // reset acceleration
    bins[bin_id].particles[i]->ax = bins[bin_id].particles[i]->ay = 0;

    // apply force within bin
    for (int j = 0; j < bins[bin_id].num_particles(); j++) {
      apply_force(*bins[bin_id].particles[i], *bins[bin_id].particles[j], dmin, davg, navg);
    }


    // apply force from edge-neighboring bins
    if (bin_x_id != 0) {
      int neighbor_id = bin_id - n_bins_per_side;
      for (int j = 0; j < bins[neighbor_id].num_particles(); j++) {
        apply_force(*bins[bin_id].particles[i], *bins[neighbor_id].particles[j], dmin, davg, navg);
      }
    }
    if (bin_x_id != n_bins_per_side-1) {
      int neighbor_id = bin_id + n_bins_per_side;
      for (int j = 0; j < bins[neighbor_id].num_particles(); j++) {
        apply_force(*bins[bin_id].particles[i], *bins[neighbor_id].particles[j], dmin, davg, navg);
      }
    }
    if (bin_y_id != 0) {
      int neighbor_id = bin_id - 1;
      for (int j = 0; j < bins[neighbor_id].num_particles(); j++) {
        apply_force(*bins[bin_id].particles[i], *bins[neighbor_id].particles[j], dmin, davg, navg);
      }
    }
    if (bin_y_id != n_bins_per_side-1) {
      int neighbor_id = bin_id + 1;
      for (int j = 0; j < bins[neighbor_id].num_particles(); j++) {
        apply_force(*bins[bin_id].particles[i], *bins[neighbor_id].particles[j], dmin, davg, navg);
      }
    }

    // apply force from edge-neighboring bins
    if (bin_x_id != 0 && bin_y_id != 0) {
      int neighbor_id = bin_id - (n_bins_per_side + 1);
      for (int j = 0; j < bins[neighbor_id].num_particles(); j++) {
        apply_force(*bins[bin_id].particles[i], *bins[neighbor_id].particles[j], dmin, davg, navg);
      }
    }
    if (bin_x_id != 0 && bin_y_id != n_bins_per_side-1) {
      int neighbor_id = bin_id - (n_bins_per_side - 1);
      for (int j = 0; j < bins[neighbor_id].num_particles(); j++) {
        apply_force(*bins[bin_id].particles[i], *bins[neighbor_id].particles[j], dmin, davg, navg);
      }
    }
    if (bin_x_id != n_bins_per_side-1 && bin_y_id != 0) {
      int neighbor_id = bin_id + n_bins_per_side - 1;
      for (int j = 0; j < bins[neighbor_id].num_particles(); j++) {
        apply_force(*bins[bin_id].particles[i], *bins[neighbor_id].particles[j], dmin, davg, navg);
      }
    }
    if (bin_x_id != n_bins_per_side-1 && bin_y_id != n_bins_per_side-1) {
      int neighbor_id = bin_id + n_bins_per_side + 1;
      for (int j = 0; j < bins[neighbor_id].num_particles(); j++) {
        apply_force(*bins[bin_id].particles[i], *bins[neighbor_id].particles[j], dmin, davg, navg);
      }
    }
  }
}


//
//  integrate the ODE
//
void move( particle_t &p )
{
    //
    //  slightly simplified Velocity Verlet integration
    //  conserves energy better than explicit Euler method
    //
    p.vx += p.ax * dt;
    p.vy += p.ay * dt;
    p.x  += p.vx * dt;
    p.y  += p.vy * dt;

    //
    //  bounce from walls
    //
    while( p.x < 0 || p.x > size )
    {
        p.x  = p.x < 0 ? -p.x : 2*size-p.x;
        p.vx = -p.vx;
    }
    while( p.y < 0 || p.y > size )
    {
        p.y  = p.y < 0 ? -p.y : 2*size-p.y;
        p.vy = -p.vy;
    }
}

//
//  I/O routines
//
void save( FILE *f, int n, particle_t *p )
{
    static bool first = true;
    if( first )
    {
        fprintf( f, "%d %g\n", n, size );
        first = false;
    }
    for( int i = 0; i < n; i++ )
        fprintf( f, "%g %g\n", p[i].x, p[i].y );
}

//
// for my own diagnosis
//
void save( FILE *f, bin_t *bins ) {
    for (int i = 0; i < n_bins_per_side; i++) {
      for (int j = 0; j < n_bins_per_side; j++) {
        fprintf(f, "bin %d , num_particles %d\n", i*n_bins_per_side+j, bins[i*n_bins_per_side+j].num_particles());
      }
    }
}

//
//  command line option processing
//
int find_option( int argc, char **argv, const char *option )
{
    for( int i = 1; i < argc; i++ )
        if( strcmp( argv[i], option ) == 0 )
            return i;
    return -1;
}

int read_int( int argc, char **argv, const char *option, int default_value )
{
    int iplace = find_option( argc, argv, option );
    if( iplace >= 0 && iplace < argc-1 )
        return atoi( argv[iplace+1] );
    return default_value;
}

char *read_string( int argc, char **argv, const char *option, char *default_value )
{
    int iplace = find_option( argc, argv, option );
    if( iplace >= 0 && iplace < argc-1 )
        return argv[iplace+1];
    return default_value;
}
