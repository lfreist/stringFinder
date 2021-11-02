# bachelorprojekt

#### Get performance on a specific expression (substring search):
```bash
./StringDataMain wikidata-people.tsv --performance "badminton"
Performance Report (expression: badminton):
 total lines:   6131837
 total matches: 21218
 query time:    0.14958 s
 time / match:  7.04967e-06 s

 ```

#### Benchmark substring search on expression
> **defaults:**
> ```
> expression:  "badminton",
> iterations:  100,
> matchCase:   false
> ```
```bash
./BenchmarkMain wikidata-people.tsv --iterations 100 --expression "badminton" --matchCase
Benchmark 'find("badminton", true)':
 Iterations: 100
 Walltime       [/s]:
  Mean:         0.148864
  Variance:     1.54338e-06
  Stddev:       0.00124233
  Max:          0.154061
  Min:          0.147468
```

## Files
| file                      | size      | lines   | usage                 | source                                                                    | Download                                                          |
|---------------------------|-----------|---------|-----------------------|---------------------------------------------------------------------------|-------------------------------------------------------------------|
| tests/data.test           | 152 bytes | 5       | simple test cases     | -                                                                         | -                                                                 |
| wikidata-people.tsv       | 547.1 MB  | 6131837 | performance/benchmark | [qlever example query](https://qlever.cs.uni-freiburg.de/wikidata/P1IH3H) | [download](https://qlever.cs.uni-freiburg.de/wikidata/P1IH3H#tsv) |

## commits that changed performance
02.11.21 22:10 - [c8f8ce78716e00ba2b108a8fe8bdba8fd9894471](https://github.com/lfreist/bachelorprojekt/commit/c8f8ce78716e00ba2b108a8fe8bdba8fd9894471)
