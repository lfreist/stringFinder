FROM ubuntu:20.04
LABEL maintainer="Leon Freist <freist@informatik.uni-freiburg.de>"
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update > /dev/null && apt-get install -y apt-utils build-essential git make vim cmake > /dev/null
RUN rm -rf /var/lib/apt/lists/*
COPY . leon-freist-bachelorprojekt
WORKDIR "leon-freist-bachelorprojekt"
RUN git submodule init && git submodule update
RUN mkdir build
WORKDIR "build"
RUN cmake -DCMAKE_BUILD_TYPE=Release .. && make -j $(nproc)
COPY bashrc bashrc
CMD ["/bin/bash", "--rcfile", "bashrc"]

# docker build -t leon-freist-bachelorprojekt .
# docker run -it -v $(pwd)/files:/inputfiles/input:ro --name leon-freist-bachelorprojekt leon-freist-bachelorprojekt