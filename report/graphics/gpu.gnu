set term png font "/usr/share/fonts/dejavu/DejaVuSerifCondensed.ttf" 23 size 1600,1200
set output 'gpu.png'
set logscale xy
set xrange [ 400:15000 ]
set yrange [ 0.04:20 ]
set xlabel "Number of particles"
set ylabel "Running time"

plot "../data/gpu_default.txt"  using 1:2 title 'default gpu code O(n^2)' with lines, \
     "../data/gpu.txt"  using 1:2 title 'gpu code O(n)' with lines
