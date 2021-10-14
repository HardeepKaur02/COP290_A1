# Gnuplot script file for plotting data
set terminal postscript eps enhanced color font 'Helvetica,18'
set output "plot2.eps"
set xlabel '{/Helvetica-Oblique Resolution factor}'
set xtics 0.1,0.1
set y2tics 95,0.4
set y2tics nomirror
set ylabel '{/Helvetica-Oblique Runtime (in s)}'
set y2label '{/Helvetica-Oblique Utility (percentage)}'
plot "method2_runtime_utility" using 1:($2/1000) axis x1y1 title "Runtime" with lines lw 2 lt 1, "method2_runtime_utility" using 1:3 axis x2y2 title "Utility" with linespoints lt 3 , "method2_runtime_utility" using 1:($4/1000) axis x1y1 title "Baseline Runtime" with lines lt 7
