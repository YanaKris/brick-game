FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        libncurses5-dev \
        libncursesw5-dev \
        libtinfo-dev \
        libgtest-dev \
        lcov \
        valgrind \
        clang-format \
        qtbase5-dev \
        qt5-qmake \
    && rm -rf /var/lib/apt/lists/*

# Ubuntu's libgtest-dev package ships sources only; build the static libs once
# so `-lgtest -lgtest_main` resolves for every test target in src/Makefile.
RUN cmake -S /usr/src/googletest -B /tmp/gtest-build \
    && cmake --build /tmp/gtest-build -j"$(nproc)" \
    && find /tmp/gtest-build -name "*.a" -exec cp {} /usr/lib \; \
    && rm -rf /tmp/gtest-build

WORKDIR /app
COPY src/ src/

WORKDIR /app/src
RUN make all

CMD ["./brickgame"]
