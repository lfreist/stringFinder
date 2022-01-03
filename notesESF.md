# Notes on ExternStringFinders Performance
#### only zstdcat:
```
$ time zstdcat f19g.txt-1.gz > /dev/null
real    0m3,288s
user    0m3,264s
sys     0m0,024s
```


#### commit [9f5a6409e1c71fb07e879203241a411e6ab3c284](https://github.com/lfreist/bachelorprojekt/commit/9f5a6409e1c71fb07e879203241a411e6ab3c284) without processing data
> out commented: src/ExternStringFinder.cpp:113
```
$ time zstdcat ../scripts/comp/f19g.txt-1.gz | ./ExternStringFinderMain keyword
real    0m6,861s
user    0m3,737s
sys     0m6,140s
```

#### commit [9f5a6409e1c71fb07e879203241a411e6ab3c284](https://github.com/lfreist/bachelorprojekt/commit/9f5a6409e1c71fb07e879203241a411e6ab3c284) with processing data:
```
$ time zstdcat ../scripts/comp/f19g.txt-1.gz | ./ExternStringFinderMain keyword --silent
real    0m7,301s
user    0m4,626s
sys     0m5,684s
```

#### commit [43197457a1907771b335a9715535203c55a418a1](https://github.com/lfreist/bachelorprojekt/commit/43197457a1907771b335a9715535203c55a418a1) (multithread):
```
$ time zstdcat ../scripts/comp/f19g.txt-1.gz | ./ExternStringFinderMain keyword --silent
real    0m7,227s
user    0m4,814s
sys     0m6,366s
```

## Just reading stdin without doing anything else:
```c++
// readStdin.cpp

#include <cstdio>

#define BUF_SIZE (2 << 15)

int main(int argc, char** argv) {
  char* buffer[BUF_SIZE];
  size_t br;
  while (true) {
    br = fread(buffer, sizeof(char), BUF_SIZE, stdin);
    if (br < 1) { break; }
  }
  return 0;
}
```
#### using zstdcat:
```
$ time zstdcat ../scripts/comp/f19g.txt-1.gz | ./readStdin
real	0m6,459s
user	0m3,578s
sys	0m5,122s
```

#### using cat
```
time cat ../scripts/f19g.txt > /dev/null
real	0m28,470s
user	0m0,045s
sys	0m7,052s
```

```
time cat ../scripts/f19g.txt | ./test.exe 
real	0m31,322s
user	0m1,161s
sys	0m14,684s
```
