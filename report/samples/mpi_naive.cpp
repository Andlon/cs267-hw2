init particles
init bins

scatter (particles, local_particles);

foreach timestep t {
  allgather(local_particles, particles);
  
  foreach particle i {
    assign_to_bin(i);
  }

  foreach local_particles i {
    bin_id b;
    foreach particle j in nearby(b) {
      compute_force (i, j);
    }
  }

  foreach particle i {
    move (i);
  }
}
