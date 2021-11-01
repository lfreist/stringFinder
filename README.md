# bachelorprojekt

#### Get performance on a specific expression (substring search):
```bash
./StringDataMain tests/wikidata-people.tsv --performance "badminton"
Performance Report (expression: badminton):
 total lines:   6131837
 total matches: 21218
 query time:    0.14958 s
 time / match:  7.04967e-06 s

 ```

#### Benchmark substring search on expression
> **defaults:**
> ```
> expression: "badminton",
> iterations: 100
> ```
```bash
./BenchmarkMain tests/wikidata-people.tsv --iterations 100 --expression "badminton"
Benchmark 'find("badminton")':
 Iterations: 100
 Walltime       [/s]:
  Mean:         0.148864
  Variance:     1.54338e-06
  Stddev:       0.00124233
  Max:          0.154061
  Min:          0.147468
```

## Files
| file                      | size      | lines   | usage                 | source                                                                    |
|---------------------------|-----------|---------|-----------------------|---------------------------------------------------------------------------|
| tests/data.test           | 152 bytes | 5       | simple test cases     | -                                                                         |
| tests/wikidata-people.tsv | 547.1 MB  | 6131837 | performance/benchmark | [qlever example query](https://qlever.cs.uni-freiburg.de/wikidata/P1IH3H) |