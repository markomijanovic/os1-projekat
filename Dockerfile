FROM ubuntu:20.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    binutils-riscv64-linux-gnu \
    g++-riscv64-linux-gnu \
    gcc-riscv64-linux-gnu \
    gdb-multiarch \
    qemu-system-misc \
    make \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /app
