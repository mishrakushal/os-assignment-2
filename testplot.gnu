set terminal png size 1280, 941
set output "testimage.png"

set datafile separator ','

set xlabel "Number of Threads"
set ylabel "Time in nanosec"
set title "Time v/s Threads Plot"

plot 'timing.csv' with lines