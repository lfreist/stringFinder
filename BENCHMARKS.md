# ExternStringFinder

## using compressed input

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