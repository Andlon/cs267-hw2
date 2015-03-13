set term png font "/usr/share/fonts/dejavu/DejaVuSerifCondensed.ttf" 23 size 1600,1200
set output 'loglogOn_largescale.png'
set xrange [ 1000:1024000 ]
set yrange [ 0.02:1000 ]
set key left top
set logscale xy
set xlabel "Number of particles"
set ylabel "Simulation time (s)"

plot "../data/largescale/serial-alternative-largescale.txt" using 1:2 title 'Serial - O(n^1.07)' with lines, \
     "../data/largescale/openmp-On3-largescale.txt" using 1:3 title 'OpenMP - 3 threads - O(n^1.07)' with lines, \
     "../data/largescale/openmp-On6-largescale.txt" using 1:3 title 'OpenMP - 6 threads - O(n^1.04)' with lines, \
     "../data/largescale/openmp-On12-largescale.txt" using 1:3 title 'OpenMP - 12 threads - O(n^1.02)' with lines, \
     "../data/largescale/openmp-On24-largescale.txt" using 1:3 title 'OpenMP - 24 threads O(n^0.96)' with lines

