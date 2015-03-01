#include<vector>

//
//  tuned constants
//
#define density 0.0005
#define mass    0.01
#define cutoff  0.01
#define min_r   (cutoff/100)
#define dt      0.0005
#define bin_size (cutoff*1)

#ifndef __CS267_COMMON_H__
#define __CS267_COMMON_H__

inline int min( int a, int b ) { return a < b ? a : b; }
inline int max( int a, int b ) { return a > b ? a : b; }

//
//  saving parameters
//
const int NSTEPS = 1000;
const int SAVEFREQ = 10;

//
// particle data structure
//
typedef struct 
{
  double x;
  double y;
  double vx;
  double vy;
  double ax;
  double ay;
} particle_t;


class bin_t
{
private:
  std::vector<particle_t *> particles;
public:
  bin_t() { /*particles.resize(100);*/ }
  void add_particle(particle_t *particle) {
    particles.push_back(particle);
  }
  void clear() {
    particles.clear();
  }
  particle_t *get_particle(int i) {
    return particles[i];
  }
  int num_particles() {
    return particles.size();
  }
};

//
//  timing routines
//
double read_timer( );

//
//  simulation routines
//
void set_size( int n );
void init_particles( int n, particle_t *p );
int init_bins (bin_t*& bins);
void clear_bins(bin_t*& bins);
void bin_particles( bin_t *bins, particle_t *particles, int n);
void bin_particles( bin_t *bins, particle_t *particles, int *particle_bin_ids, int n);
void apply_force( particle_t &particle, particle_t &neighbor , double *dmin, double &davg, int &navg);
void apply_force_particle_bin( particle_t &particle, int bin_id, bin_t *bins, double *dmin, double &davg, int &navg);
void apply_force_bin( bin_t *bins, int bin_id, double *dmin, double &davg, int &navg);
void move( particle_t &p );


//
//  I/O routines
//
FILE *open_save( char *filename, int n );
void save( FILE *f, int n, particle_t *p );
void save( FILE *f, bin_t *bins );

//
//  argument processing routines
//
int find_option( int argc, char **argv, const char *option );
int read_int( int argc, char **argv, const char *option, int default_value );
char *read_string( int argc, char **argv, const char *option, char *default_value );

#endif
