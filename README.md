# bachelorprojekt
Search file for substrings.

## Usage:
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && make -j $(nproc)
```
### Use docker
```bash
docker build -t leon-freist-bachelorprojekt .
docker run -it -v $(pwd)/files:/inputfiles/input:ro --name leon-freist-bachelorprojekt leon-freist-bachelorprojekt
```

### StringFinderInteractiveMain
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
>  find EXPRESSION {--ARGUMENT}s -> find EXPRESSION in currentloaded file
>   --performance    display performance of a single search
>   --lines n        display first n lines
>  load FILE {--ARGUMENT}s       -> load FILE (delete old data)
>   --append         append the data of FILE to the old data
>  help                          -> display this guide
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
