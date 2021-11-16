# TODOs defined in meetings

## 11.11.2021
- Load file into RAM and search:
  - [x] fix Dockerfile
  - [x] implement interactive search
  - [x] parallelize the StringData.find() method using openmp
  - [ ] show number of matching rows and total number of rows in benchmark
- Search on external data:
  - [ ] integrate GNU grep as baseline for StringFinder performance
  - [ ] implement a data structure that allows "grep-like" searching but knows the line indices of matches