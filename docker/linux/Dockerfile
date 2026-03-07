FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    clang-19 \
    libc++-19-dev \
    libc++abi-19-dev \
    uuid-dev \
    make \
    python3 \
    python3-dev \
    python3-pip \
    wget \
    ca-certificates \
    && CMAKE_ARCH=$(uname -m) \
    && wget -qO /tmp/cmake.sh https://github.com/Kitware/CMake/releases/download/v3.31.6/cmake-3.31.6-linux-${CMAKE_ARCH}.sh \
    && sh /tmp/cmake.sh --skip-license --prefix=/usr/local \
    && rm /tmp/cmake.sh \
    && rm -rf /var/lib/apt/lists/* \
    && pip3 install pybind11 --break-system-packages

ENV CC=clang-19
ENV CXX=clang++-19

WORKDIR /workspace

CMD ["bash", "-c", "mkdir -p cmake-build-linux && cd cmake-build-linux && cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=\"-stdlib=libc++ -fPIC\" -DCMAKE_EXE_LINKER_FLAGS=\"-stdlib=libc++\" && cmake --build . -j 8"]