set terminal png size 800,500
set output 'output.png'
set style data histogram
set style histogram cluster gap 1
set style fill solid
set boxwidth 0.9
set xtics format ""
set grid ytics

set title "GNU grep vs ESF"

