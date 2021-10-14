# Gnuplot script file for plotting data
set terminal postscript eps enhanced color font 'Helvetica,18'
set output "plot4.eps"
set xlabel '{/Helvetica-Oblique Number of threads}'
set y2tics 
set y2tics nomirror
set ylabel '{/Helvetica-Oblique Runtime (in s)}'
set y2label '{/Helvetica-Oblique Utility (percentage)}'
plot "method4_runtime_utility" using 1:($2/1000) axis x1y1 title "Runtime" with lines lw 2 lt 1, "method4_runtime_utility" using 1:3 axis x2y2 title "Utility" with lines lt 3 , "method4_runtime_utility" using 1:($4/1000) axis x1y1 title "Baseline Runtime" with lines lt 7
