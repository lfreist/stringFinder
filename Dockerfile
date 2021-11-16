FROM ubuntu:20.04
LABEL maintainer="Leon Freist <freist@informatik.uni-freiburg.de>"
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update > /dev/null && apt-get install -y apt-utils make vim build-essential libgtest-dev valgrind cmake python3 > /dev/null
RUN rm -rf /var/lib/apt/lists/*
RUN cmake /usr/src/gtest/CMakeLists.txt > /dev/null && make -C /usr/src/gtest > /dev/null
COPY . leon-freist-bachelorprojekt
WORKDIR "leon-freist-bachelorprojekt"
RUN make compile
COPY bashrc bashrc
CMD ["/bin/bash", "--rcfile", "bashrc"]

# docker build -t leon-freist-bachelorprojekt .
# docker run -it -v $(pwd)/files:/inputfiles/input:ro --name leon-freist-bachelorprojekt leon-freist-bachelorprojekt