# plot_transpose.gp

# ------------------------------------------------------------

# Script gnuplot per visualizzare i risultati della trasposizione

# ------------------------------------------------------------

# dimensione cache (modificare se necessario)

L1 = 32*1024
L2 = 256*1024

# working set ≈ 16*N^2 bytes (2 matrici di double)

L1N = sqrt(L1/16)
L2N = sqrt(L2/16)

# stile generale

set terminal pngcairo size 1000,700 enhanced font ",14"
set grid
set key left top

# ------------------------------------------------------------

# Grafico 1: Bandwidth

# ------------------------------------------------------------

set output "transpose_bandwidth.png"

set title "Matrix Transpose Memory Bandwidth"
set xlabel "Matrix size N"
set ylabel "Bandwidth (MB/s)"

set logscale x 2

set arrow 1 from L1N, graph 0 to L1N, graph 1 nohead lw 2 dt 2 lc rgb "gray"
set arrow 2 from L2N, graph 0 to L2N, graph 1 nohead lw 2 dt 2 lc rgb "gray"

set label 1 "L1 cache limit" at L1N*1.1, graph 0.9
set label 2 "L2 cache limit" at L2N*1.1, graph 0.8

plot "transpose_strided_write.dat" using 1:2 with linespoints lw 2 pt 7 title "strided write", "transpose_contiguous_write.dat" using 1:2 with linespoints lw 2 pt 5 title "contiguous write"

unset label
unset arrow

# ------------------------------------------------------------

# Grafico 2: Speedup

# ------------------------------------------------------------

set output "transpose_speedup.png"

set title "Performance Improvement from Better Locality"
set xlabel "Matrix size N"
set ylabel "Speedup (contiguous / strided)"

set logscale x 2

plot "< paste transpose_strided_write.dat transpose_contiguous_write.dat" using 1:($5/$2) with linespoints lw 2 pt 7 title "speedup"

unset logscale

