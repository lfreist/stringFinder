# Benchmarks

## Algorithms

### using strstr and strchr

```
$ ./cmake-build-debug/strstrSearchBenchmark files/1gb.test.txt --patterns keyword lkt
algorithm                pattern                matches       time [s]
----------------------------------------------------------------------
strstr (per line)        keyword                    625        90.4014
strstr (per line)        lkt                       1472        210.495
strstr (all)             keyword                    625         45.011
strstr (all)             lkt                       1473        103.904
```

## ExternStringFinder

### using compressed input

> test.txt has a size of about 25gb and was randomly generated using [createTestFile.py](scripts/createTestFile.py) with files/words.txt
>
> test.esf.zst has a size of about 12gb and was created using ESFCompressorMain ([section 2.2.2](README.md))
>
> test.zst has a size of about 12gb and is creating using ESFCompressorMain ([section 2.2.2](README.md))

1. using zstdcat and grep

```
time zstdcat test.zst | grep keyword -c
15437

real  0m45,190s
user  0m45,535s
sys   0m9,220s
```

2. using zstdcat and ExternStringFinder
```
time zstdcat test.zst | ./ExternStringFinderMain keyword --count
Found 15437 matches
Bytes: 94688937734636

real  0m41,531s
user  0m36,775s
sys   0m11,462s

```
3. using metafile
```
time ./ExternStringFinderMain keyword test.esf.zst --meta test.esf.meta --count
Found 15437 matches
Bytes: 94156004538597

real  0m29,046s
user  0m43,214s
sys   0m3,764s
```

#### amur:
```
freist@amur:.$ time grep -c badminton wikidata.20210610.vocabulary
9824

real    4m58.910s
user    0m7.128s
sys     0m6.825s

freist@amur:.$ time ./ExternStringFinderMain badminton wikidata.20210610.vocabulary-l3-cs23.zst --meta wikidata.20210610.vocabulary-l3-cs23.zst.meta --count
Found 9824 matches

real    0m46.546s
user    0m26.191s
sys     0m2.204s
```
