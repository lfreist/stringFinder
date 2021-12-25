#!/bin/bash

function compress() {
    file=$1
    for i in $(seq 1 9); do
      out="comp/$file-$i.gz"
      zstd "$file" -$i -o "$out" -T0 > /dev/null & wait;
    done
}

if [ $# -eq 0 ]; then
  echo "Usage: compress.sh file/to/compress.file"
  exit 1
else
  file=$1
fi

if [[ -f $file ]]; then
  echo "compressing file '$file'..."
  compress "$file"
else
  echo "Error: '$file' is not a file!"
  exit 2
fi
