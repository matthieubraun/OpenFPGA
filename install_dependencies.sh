#!/usr/bin/env bash

# Add necessary repositories
add-apt-repository -y ppa:ubuntu-toolchain-r/test

# Update package list
apt-get update

# Install dependencies required to build the code base
apt-get install -y \
    autoconf \
    automake \
    bison \
    ccache \
    cmake \
    ctags \
    curl \
    doxygen \
    flex \
    fontconfig \
    gdb \
    git \
    gperf \
    iverilog \
    libc6-dev \
    libcairo2-dev \
    libevent-dev \
    libffi-dev \
    libfontconfig1-dev \
    liblist-moreutils-perl \
    libncurses5-dev \
    libreadline-dev \
    libreadline8 \
    libx11-dev \
    libxft-dev \
    libxml++2.6-dev \
    make \
    perl \
    pkg-config \
    python3 \
    python3-setuptools \
    python3-lxml \
    python3-pip \
    qt5-default \
    tcllib \
    tcl8.6-dev \
    texinfo \
    time \
    valgrind \
    wget \
    zip \
    swig \
    expect \
    g++-7 \
    gcc-7 \
    g++-8 \
    gcc-8 \
    g++-9 \
    gcc-9 \
    g++-10 \
    gcc-10 \
    g++-11 \
    gcc-11 \
    clang-6.0 \
    clang-7 \
    clang-8 \
    clang-10 \
    clang-format-10 \
    libxml2-utils \
    libssl-dev

# Update package list again as required by some packages
apt-get update

# Install dependencies required to run regression tests
apt-get install --no-install-recommends -y \
    libdatetime-perl libc6 libffi-dev libgcc1 libreadline8 libstdc++6 \
    libtcl8.6 tcl python3.8 python3-pip zlib1g libbz2-1.0 \
    iverilog git rsync make curl wget tree python3.8-venv

# Install dependencies required to build documentation
apt-get install -y python3-sphinx

# Install Python packages from requirements.txt
python3 -m pip install -r requirements.txt

# Install Python packages from docs/requirements.txt
python3 -m pip install -r docs/requirements.txt