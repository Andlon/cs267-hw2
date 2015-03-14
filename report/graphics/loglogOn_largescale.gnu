set term png font "/usr/share/fonts/dejavu/DejaVuSerifCondensed.ttf" 23 size 1600,1200
set output 'loglogOn_largescale.png'
set xrange [ 1000:1024000 ]
set yrange [ 0.02:1000 ]
set key right bottom
set logscale xy
set xlabel "Number of particles"
set ylabel "Simulation time (s)"
set style function linespoints

plot "../data/largescale/mpi-On4-largescale.txt" using 1:3 title 'MPI - 4 tasks O(n^x)' pt 7 ps 2 with linespoints, \
     "../data/largescale/mpi-On24-largescale.txt" using 1:3 title 'MPI - 24 tasks O(n^x)' pt 7 ps 2 with linespoints, \
     "../data/largescale/mpi-On96-largescale.txt" using 1:3 title 'MPI - 96 tasks O(n^x)' pt 7 ps 2 with linespoints, \
     "../data/largescale/serial-alternative-largescale.txt" using 1:2 title 'Serial - O(n^1.07)' pt 7 ps 2 with linespoints, \
     "../data/largescale/openmp-On3-largescale.txt" using 1:3 title 'OpenMP - 3 threads - O(n^1.07)' pt 7 ps 2 with linespoints, \
     "../data/largescale/openmp-On6-largescale.txt" using 1:3 title 'OpenMP - 6 threads - O(n^1.04)' pt 7 ps 2 with linespoints, \
     "../data/largescale/openmp-On12-largescale.txt" using 1:3 title 'OpenMP - 12 threads - O(n^1.02)' pt 7 ps 2 with linespoints, \
     "../data/largescale/openmp-On24-largescale.txt" using 1:3 title 'OpenMP - 24 threads O(n^0.96)' pt 7 ps 2 with linespoints

