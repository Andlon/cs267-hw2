set term png font "/usr/share/fonts/dejavu/DejaVuSerifCondensed.ttf" 23 size 1600,1200
set output 'speedup-16000.png'
#set xrange [ 400:12000 ]
#set yrange [ 0.02:20 ]
set xlabel "Number of threads"
set ylabel "Speedup"

plot "../data/openmp24-16000.txt"  using 1:2 title 'speedup for openMP' with lines, \
     "../data/pthreads24-16000.txt"  using 1:2 title 'speedup for pthreads' with lines, \
     "../data/mpi24-16000.txt"  using 1:2 title 'speedup for MPI' with lines
