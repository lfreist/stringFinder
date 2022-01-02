#!/bin/bash

function progress() {
  echo -ne "Progress: $1 \r"
}

function run_benchmark() {
  files_processed=0
  out="results.tsv"
  if [ ! -f "$out" ]; then
    touch "$out"
  fi
  header="#"
  m_a="$_a"
  m_b="$_b"
  for file in $dir; do
    if [ -f "$file" ] && [[ "$file" == *.gz ]]; then
      files_processed=$(("$files_processed" + 1))
      progress $(("$files_processed"))
      header="$header\t$(basename "$file")"
      m_a="$m_a\t$(TIMEFORMAT='%R'; { time zstdcat "$file" | $_a "$keyword" > /dev/null; } 2>&1)"
      m_b="$m_b\t$(TIMEFORMAT='%R'; { time zstdcat "$file" | $_b "$keyword" > /dev/null; } 2>&1)"
    fi
  done;
  echo -e "$header" > "$out"
  echo -e "$m_a" | tr , . >> "$out"
  echo -e "$m_b" | tr , . >> "$out"
}

function writeGnuPlotScript() {
  counter=2
  plot=""
  if [ -f "plot_time.gnuplot" ]; then
    rm -f "plot_time.gnuplot"
  fi
  cp plot_time_template.gnuplot plot_time.gnuplot
  for file in $dir; do
    if [ -f "$file" ] && [[ "$file" == *.gz ]]; then
      if [[ "$counter" -eq 2 ]]; then
        plot="plot 'results.tsv' using $counter:xtic(1) title '$(basename "$file")'"
      else
        plot="$plot, 'results.tsv' using $counter title '$(basename "$file")'"
      fi
    fi
    counter=$(("$counter" + 1))
  done
  echo -e "set title \"$_a vs $_b\"\n\n" >> "plot_time.gnuplot"
  echo "$plot" >> "plot_time.gnuplot"
}

if [ ! $# -eq 4 ]; then
  echo "Usage: benchmark.sh /dir/with/compressed/files 'keyword' /path/to/a /path/to/b"
  exit 1
else
  if [[ "$1" == */ ]]; then
    dir="$1*"
  else
    dir="$1/*"
  fi
  keyword="$2"
  _a="$3"
  _b="$4"
fi

run_benchmark
writeGnuPlotScript
gnuplot plot_time.gnuplot
