init particles
init bins

init locks

foreach timestep t {
  parallel foreach bin b {
    clear (b);
  }

  parallel foreach particle i {
    compute_bin_id b;
    lock (b);
    assign_to_bin (i, b);
    unlock (b);
  }

  parallel foreach bin b {
    compute_forces_in_bin (b);
  }

  parallel foreach particle i {
    move (i);
  }
}
