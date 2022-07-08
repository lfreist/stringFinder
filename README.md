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
### API
#### StringFinder
> `StringFinder` is

#### TaskAndNumThreads
> The `TaskAndNumThreads` struct is used for task initialization. Since all its attributes are copyable and movable,
> tasks can simply be passed as a vector of `TaskAndNumThreads` to the StringFinder, while the `Task` is neither movable
> nor copyable.
```c++
// src/StringFinder.h

struct TaskAndNumThreads {
  std::string name;
  std::function<void(sf::utils::FileChunk *)> task;
  unsigned numThreads;
}
```

### 2.1 ExternStringFinder (ESF)

#### 2.2.2 Increase ESF performance using ESFCompressorMain
1. Compress the file using ESFCompressorMain
```
ESFCompressorMain SRC DEST [META]
```
2. Run ESF with the metafile option
```
./ExternStringFinder KEYWORD SRC-FILE --meta META-FILE
```
