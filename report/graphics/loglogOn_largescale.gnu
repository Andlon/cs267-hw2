set term png font "/usr/share/fonts/dejavu/DejaVuSerifCondensed.ttf" 23 size 1600,1200
set output 'loglogOn_largescale.png'
set xrange [ 1000:1024000 ]
set yrange [ 0.02:1000 ]
set logscale xy
set xlabel "Number of particles"
set ylabel "Simulation time (s)"

plot "../data/largescale/serial-alternative-largescale.txt" using 1:2 title 'Serial' with lines, \
     "../data/largescale/openmp-On6-largescale.txt" using 1:3 title 'OpenMP - 6 threads' with lines
#     "../data/mpi_naiveOn1.txt" using 1:3 title 'mpi with 1 node' with lines, \
#     "../data/mpi_naiveOn4.txt" using 1:3 title 'mpi with 4 nodes' with lines
