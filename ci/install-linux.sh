#!/usr/bin/env bash

if [ "$TRAVIS_OS_NAME" != "linux" ]; then
    echo "Not a Linux build; skipping installation"
    exit 0;
fi

DEPS_DIR="${TRAVIS_BUILD_DIR}/deps"

mkdir ${DEPS_DIR} && cd ${DEPS_DIR}

wget --no-check-certificate https://cmake.org/files/v3.12/cmake-3.12.3-Linux-x86_64.tar.gz
tar -xvf cmake-3.12.3-Linux-x86_64.tar.gz > /dev/null
mv cmake-3.3.2-Linux-x86_64 cmake-install
PATH=${DEPS_DIR}/cmake-install:${DEPS_DIR}/cmake-install/bin:$PATH
cd ${TRAVIS_BUILD_DIR}