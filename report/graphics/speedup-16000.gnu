set term png font "/usr/share/fonts/dejavu/DejaVuSerifCondensed.ttf" 23 size 1600,1200
set output 'speedup-16000.png'
#set xrange [ 400:12000 ]
set yrange [ 0.02:8 ]
set xlabel "Number of threads"
set ylabel "Speedup"

plot "../data/ideal_n16000.txt" using 1:2 title 'Idealized p-times speedup' pt 7 ps 2 with linespoints, \
     "../data/openmp24-16000.txt"  using 1:2 title 'Speedup for OpenMP' pt 7 ps 2 with linespoints, \
     "../data/pthreads24-16000.txt"  using 1:2 title 'Speedup for Pthreads' pt 7 ps 2 with linespoints, \
     "../data/mpi24-16000.txt"  using 1:2 title 'Speedup for MPI' pt 7 ps 2 with linespoints
