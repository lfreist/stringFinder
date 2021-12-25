set terminal png size 800,500
set output 'output.png'
set style data histogram
set style histogram cluster gap 1
set style fill solid
set boxwidth 0.9
set xtics format ""
set grid ytics

set title "GNU grep vs ESF"

plot 'results.tsv' using 2:xtic(1) title 'f19g.txt-1.gz', 'results.tsv' using 3 title 'f19g.txt-2.gz', 'results.tsv' using 4 title 'f19g.txt-3.gz', 'results.tsv' using 5 title 'f19g.txt-4.gz', 'results.tsv' using 6 title 'f19g.txt-5.gz', 'results.tsv' using 7 title 'f19g.txt-6.gz', 'results.tsv' using 8 title 'f19g.txt-7.gz', 'results.tsv' using 9 title 'f19g.txt-8.gz', 'results.tsv' using 10 title 'f19g.txt-9.gz'
