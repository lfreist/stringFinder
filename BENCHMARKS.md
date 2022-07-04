# Benchmarks

## Algorithms

_find_ the code:
- [strstr](src/string_search_algorithms/using_strstr.h)
- [stdstr](src/string_search_algorithms/using_stdstring.h)

```
$ ./stringSearchBenchmark files/1gb.test.txt --patterns keyword lkt --algorithms all
algorithm                pattern                matches       time [s]
----------------------------------------------------------------------
strstr (per line)        keyword                    625        52.9024
strstr (per line)        lkt                       1472       123.6570
strstr (all)             keyword                    625        42.1696
strstr (all)             lkt                       1473        99.2842
stdstring (per line)     keyword                    625         0.1336
stdstring (per line)     lkt                       1472         0.3938
stdstring (all)          keyword                    625         0.1347
stdstring (all)          lkt                       1473         0.3923
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

# with commit [b930693](https://github.com/lfreist/stringFinder/commit/b930693355430a12eb8b6e61faf8bb95adfb58ab)
> rewrote string finder with tasks etc

####  [b930693](https://github.com/lfreist/stringFinder/commit/b930693355430a12eb8b6e61faf8bb95adfb58ab)
```
lfreist@think:~/CLionProjects/stringFinder/cmake-build-release$ time ./ExternStringFinderMain keyword ../files/5gb.dummy.txt -c -p

StringFinder - Performance Report
 └─ Total Time (real): 13.634603 s
    └─ threads:  14.324115
       ├─ Reading: 13.634293 s
       │  ├─ reading: 13.634238 s
       │  └─ waiting: 0.000055 s
       └─ Searcher (1 thread): 13.634465 s
          ├─ computing: 0.689512 s
          └─ waiting: 12.784224 s
             ├─ push: 0.000000 s
             └─ pull: 12.784224 s

3179

real    0m13,645s
user    0m2,011s
sys     0m6,697s
```
#### grep
```
lfreist@think:~/CLionProjects/stringFinder/cmake-build-release$ time grep keyword ../files/5gb.dummy.txt -c
3179

real    0m10,249s
user    0m2,398s
sys     0m0,975s
```
#### latest commit before rewriting SF [b41502f](https://github.com/lfreist/stringFinder/commit/b41502fa74403a1630a004966c4ba6d16a776c94)
```
lfreist@think:/tmp/stringFinder/build$ time ./ExternStringFinderMain keyword ~/CLionProjects/stringFinder/files/5gb.dummy.txt -c -p
ExternStringFinder::find() on keyword 'keyword' and inputfile '/home/lfreist/CLionProjects/stringFinder/files/5gb.dummy.txt'.


Performance report of ExternStringFinder::find()
 └─ 10.788247 s in total (processor time: 13.784090 s)
    ├─ reading input (1 thread): 10.773537 s
    │  ├─ computation: 10.773342 (78%)
    │  └─ waiting    : 0.000195 (0%)
    └─ searching: (1 threads): 10.786249 s
       ├─ computation: 3.010747 (21%)
       └─ waiting    : 7.775502 (99%)

3179

real	0m10,812s
user	0m3,119s
sys	0m4,602s
```