ARG GCC_VERSION=14
ARG USERNAME=ubuntu
# Default GCC version

FROM ubuntu:24.04
# Use the official Ubuntu 24.04 image as a base

ARG GCC_VERSION
ARG USERNAME

RUN apt update && \
    apt install -y sudo && \
    echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/${USERNAME} && \
    chmod 0440 /etc/sudoers.d/${USERNAME}

# Add cmake repository and install cmake
RUN apt update && \
    apt install -y ca-certificates gpg wget ninja-build sudo && \
    wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - >/usr/share/keyrings/kitware-archive-keyring.gpg && \
    echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ noble main' > /etc/apt/sources.list.d/kitware.list && \
    apt-get update && \
    apt update && \
    apt install -y cmake && \
    rm -rf /var/lib/apt/lists/*

RUN apt update && \
    apt install -y software-properties-common && \
    add-apt-repository ppa:ubuntu-toolchain-r/test && \
    apt update && \
    apt install -y gcc-${GCC_VERSION} g++-${GCC_VERSION} gdb && \
    rm -rf /var/lib/apt/lists/*
COPY ld.so.conf /etc/ld.so.conf.d/g++.conf


RUN apt update && \
    add-apt-repository ppa:git-core/ppa && \
    apt update && \
    apt install -y git && \
    rm -rf /var/lib/apt/lists/*

ADD --chmod=0644 CMakeToolchain.txt /CMakeToolchain.txt
ENV CMAKE_TOOLCHAIN_FILE="/CMakeToolchain.txt"

# [Optional] Set the default user. Omit if you want to keep the default as root.
USER ${USERNAME}
