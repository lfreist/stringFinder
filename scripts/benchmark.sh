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
  m_grep="grep"
  m_esf="esf"
  for file in $dir; do
    if [ -f "$file" ] && [[ "$file" == *.gz ]]; then
      files_processed=$(("$files_processed" + 1))
      progress $(("$files_processed"))
      header="$header\t$(basename "$file")"
      m_esf="$m_esf\t$(TIMEFORMAT='%R'; { time zstdcat "$file" | $_esf "$keyword" --count --silent > /dev/null; } 2>&1)"
      m_grep="$m_grep\t$(TIMEFORMAT='%R'; { time zstdcat "$file" | $_grep "$keyword" > /dev/null; } 2>&1)"
    fi
  done;
  echo -e "$header" > "$out"
  echo -e "$m_esf" | tr , . >> "$out"
  echo -e "$m_grep" | tr , . >> "$out"
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
  echo "$plot" >> "plot_time.gnuplot"
}

if [ ! $# -eq 4 ]; then
  echo "Usage: benchmark.sh /dir/with/compressed/files 'keyword' /path/to/grep /path/to/esf"
  exit 1
else
  if [[ "$1" == */ ]]; then
    dir="$1*"
  else
    dir="$1/*"
  fi
  keyword="$2"
  _grep="$3"
  _esf="$4"
fi

run_benchmark
writeGnuPlotScript
gnuplot plot_time.gnuplot
