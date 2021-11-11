FROM ubuntu:20.04
LABEL maintainer="Leon Freist <freist@informatik.uni-freiburg.de>"
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && apt-get install -y apt-utils make vim build-essential libgtest-dev valgrind cmake python3
RUN rm -rf /var/lib/apt/lists/*
RUN cmake /usr/src/gtest/CMakeLists.txt && make -C /usr/src/gtest
COPY . leon-freist-bachelorprojekt
COPY bashrc bashrc
CMD ["/bin/bash", "--rcfile", "bashrc"]

# docker build -t leon-freist-bachelorprojekt .
# docker run -it -v $(pwd)/files:/inputfiles/input:ro --name leon-freist-bachelorprojekt leon-freist-bachelorprojekt