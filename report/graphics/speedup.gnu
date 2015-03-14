set term png font "/usr/share/fonts/dejavu/DejaVuSerifCondensed.ttf" 23 size 1600,1200
set output 'speedup.png'
#set xrange [ 400:12000 ]
set yrange [ 0.02:4 ]
set xlabel "Number of threads/tasks"
set ylabel "Speedup"
set key right top

plot "../data/ideal_n500.txt" using 1:2 title 'Idealized p-times speedup' pt 7 ps 2 with lines, \
     "../data/openmp24.txt"  using 1:2 title 'Speedup for OpenMP' pt 7 ps 2 with linespoints, \
     "../data/pthreads24.txt"  using 1:2 title 'Speedup for Pthreads' pt 7 ps 2 with linespoints, \
     "../data/mpi24.txt"  using 1:2 title 'Speedup for MPI' pt 7 ps 2 with linespoints
