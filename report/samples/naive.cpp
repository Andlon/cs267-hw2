init particles
foreach timestep t {
  foreach particle i {
    foreach particle j {
      compute_force (i, j);
    }
  }
  foreach particle i {
    move (i);
  }
}

