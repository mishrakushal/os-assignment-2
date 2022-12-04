set terminal png size 1280, 941
set output "p2Plot.png"

set datafile separator ','

set xlabel "Number of Threads"
set ylabel "Time in nanosec"
set title "Time for Matrix Multiplication"

plot 'p2_timing.csv' with lines