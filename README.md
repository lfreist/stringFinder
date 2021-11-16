# bachelorprojekt
Search file for substrings.
## Usage:
```bash
# only build binaries:
make compile

# run compile, checkstyle, valgrind, tests:
make all
```
### StringFinderInteractiveMain
This will start an interactive console reading the provided `file` (where `file` is optional and either a relative or an absolute path to a file).

```
./StringFinderInteractiveMain file
Reading file file
done
>
```
> You can now enter commands with arguments:
> ```
> Usage: [COMMAND] {OBJECT} {--ARGUMENT {INT}}S
>  Commands:
>   exit    no object    no arguments          exit the interactive mode
>   find    expression   performance, lines n  search 'expression' in current loaded file
>   laod    file         no arguments          load 'file' (overwrite current loaded file)
>   help    no object    no arguments          print this usage guide
>
>  Examples:
>   find expression
>   find "expre ssion"
>   find expre-ssion
>   find expression --lines 5 --performance
>   load /path/to/file
> ```

```
> find expression --performance
Searching for 'expression'...
Performance Report:
StringFinder.measurePerformance(expression, 0):
 total lines:   6131837
 total matches: 2
 query time:    0.825045 s
 time / match:  0.412523 s
```

## Files
| file                      | size      | lines   | usage                 | source                                                                    | Download                                                          |
|---------------------------|-----------|---------|-----------------------|---------------------------------------------------------------------------|-------------------------------------------------------------------|
| tests/data.test           | 152 bytes | 5       | simple test cases     | -                                                                         | -                                                                 |
| wikidata-people.tsv       | 547.1 MB  | 6131837 | performance/benchmark | [qlever example query](https://qlever.cs.uni-freiburg.de/wikidata/P1IH3H) | [download](https://qlever.cs.uni-freiburg.de/wikidata/P1IH3H#tsv) |

## commits that changed performance

02.11.21 22:10 - [c8f8ce78716e00ba2b108a8fe8bdba8fd9894471](https://github.com/lfreist/bachelorprojekt/commit/c8f8ce78716e00ba2b108a8fe8bdba8fd9894471) - first implementation

02.11.21 22:30 - [431333bd8bd4e4784e869fce929ac6db5329bd3f](https://github.com/lfreist/bachelorprojekt/commit/431333bd8bd4e4784e869fce929ac6db5329bd3f) - don't copy strings in find for matchCase

05.11.21 09:40 - [53f4113e4d668c241f2e76c21e0ea90b28cbf3c3](https://github.com/lfreist/bachelorprojekt/commit/53f4113e4d668c241f2e76c21e0ea90b28cbf3c3) - only return pointers to matched strings from find()

16.11.21 12:54 - [4f9f57f0470e34827afe36d92051ff66a5898d18](https://github.com/lfreist/bachelorprojekt/commit/4f9f57f0470e34827afe36d92051ff66a5898d18) - added multithread search
