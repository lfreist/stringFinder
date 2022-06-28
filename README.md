# stringFinder (bachelorprojekt)
Search a file for a keyword.

> Benchmarks: [BENCHMARKS.md](BENCHMARKS.md)

## 1. Build:
```bash
git clone https://github.com/lfreist/bachelorprojekt.git
# only for standalone software:
git submodule init
git submodule update
```
### 1.1 Standalone Software
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && make -j $(nproc)
```
> Note: When compiling on MacOS, install g++ using brew and add ```-DCMAKE_CXX_COMPILER=/usr/local/bin/c++-11``` to the cmake call

### 1.2 Docker
```
docker build -t leon-freist-bachelorprojekt .
docker run -it -v $(pwd)/files:/inputfiles/input:ro --name leon-freist-bachelorprojekt leon-freist-bachelorprojekt
```

## 2. Usage

### 2.1 StringFinderInteractiveMain (SFI)
This will start an interactive console reading the provided `file` (where `file` is optional and either a relative or an absolute path to a file).

```
./StringFinderInteractiveMain file
Reading file file
done
>
```
> Usage:
> ```
> Usage: [COMMAND] {OBJECT} {--ARGUMENT {INT}}s
> Interactively search for substrings in a file
> 
> Commands:
>  exit                          -> exit interactive mode
>  find EXPRESSION {--ARGUMENT}s -> _find_ EXPRESSION in currentloaded file
>   --performance    display performance of a single search
>   --lines n        display first n lines
>  load FILE {--ARGUMENT}s       -> load FILE (delete old data)
>   --append         append the data of FILE to the old data
>  help                          -> display this guide
> ```

```
> _find_ expression --performance
Searching for 'expression'...
Performance Report:
InternStringFinder.measurePerformance(expression, 0):
 total lines:   6131837
 total matches: 2
 query time:    0.825045 s
 time / match:  0.412523 s
```

### 2.2 ExternStringFinder (ESF)

ESF basically works as the (non regex) grep command:

```
InternStringFinder - ExternStringFinder - Leon Freist <freist@informatik.uni-freibur.de>
Usage: ./ExternStringFinderMain [PATTERN] [FILE] [OPTION]...
 Search for a PATTERN in a FILE.
 Example: ./ExternStringFinderMain 'hello world' main.c
If you provide a (zstd-)compressed file as input, also set a meta file using the --meta flag.

  OPTIONS:
  --help         -h  print this guide and exit.
  --performance  -p  measure wall time on _find_ and print result.
  --silent       -s  dont print matching lines.
  --count        -c  print number of matching lines.
  --meta [FILE]  -m  set meta file.

When FILE is not provided read standard input
 Example: cat main.c | ./ExternStringFinderMain 'hello world'
```

#### 2.2.1Benchmarking ESF vs grep
From within the `./scripts` directory run these tasks:
```bash
python3 benchmark.py [FILE] [KEYWORD] --programs [GREP] [ESF] [...] --iterations [X]
# where [FILE]    is a zstd compressed file
#       [KEYWORD] is a single word to be searched during the benchmarks,
#       [GREP]    is the path to grep (e.g. /bin/grep)
#       [ESF]     is the path to ESF (e.g. ../build/ExternStringFinderMain)
#       [X]       is the number of runs per program
```

#### 2.2.2 Increase ESF performance using ESFCompressorMain
1. Compress the file using ESFCompressorMain
```
ESFCompressorMain SRC DEST [META]
```
2. Run ESF with the metafile option
```
./ExternStringFinder KEYWORD SRC-FILE --meta META-FILE
```
