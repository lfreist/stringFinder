FROM ubuntu:20.04
LABEL maintainer="Leon Freist <freist@informatik.uni-freiburg.de>"
RUN apt-get update && apt-get install -y apt-utils make vim build-essential libgtest-dev valgrind
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends tzdata
RUN apt-get install cmake
RUN rm -rf /var/lib/apt/lists/*
RUN cmake /usr/src/gtest/CMakeLists.txt && make -C /usr/src/gtest
COPY . leon-freist-bachelorprojekt
COPY bashrc bashrc
CMD ["/bin/bash", "--rcfile", "bashrc"]

# docker build -t leon-freist-bachelorprojekt .
# docker run -it -v $(pwd)/files:/inputfiles/input:ro --name leon-freist-bachelorprojekt leon-freist-bachelorprojekt