init particles
init bins

foreach timestep t {
  foreach particle i {
    assign_to_bin (i);
  }
  foreach bin b {
    foreach particle i in b {
      foreach particle j in nearby(b) {
        compute_force (i, j);
      }
    }
  }
  foreach particle i {
    move (i);
  }
}
