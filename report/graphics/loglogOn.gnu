set term png font "/usr/share/fonts/dejavu/DejaVuSerifCondensed.ttf" 23 size 1600,1200
set output 'loglogOn.png'
set xrange [ 400:12000 ]
set yrange [ 0.02:20 ]
set logscale xy
set xlabel "Number of particles"
set ylabel "Simulation time (s)"

plot "<(grep 'n =' ../data/auto-serial.stdout | tr ',' ' ')" using 3:7 title 'Serial code O(n^2)' with lines, \
     "<(grep 'n =' ../data/auto-serial-linear.stdout | tr ',' ' ')" using 3:7 title 'Serial code O(n)' with lines, \
     "../data/openmpOn1.txt" using 1:3 title 'openmp with 1 thread' with lines, \
     "../data/openmpOn4.txt" using 1:3 title 'openmp with 4 threads' with lines, \
     "../data/mpi_naiveOn1.txt" using 1:3 title 'mpi with 1 node' with lines, \
     "../data/mpi_naiveOn4.txt" using 1:3 title 'mpi with 4 nodes' with lines
