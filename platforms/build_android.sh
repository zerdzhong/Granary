#!/usr/bin/env bash

if ! [ -x "$(command -v cmake)" ]; then
    echo "Error: cmake is not installed.\nFollow https://cmake.org/install/ this web page to install cmake newer than 3.12.0" >&2
    exit 1
fi

if [ ! -d "${ANDROID_NDK}" ]; then
    ANDROID_NDK=~/Library/Android/sdk/ndk-bundle
fi

if [ ! -d "${ANDROID_NDK}" ]; then
    echo "Error: Make sure install ndk and set ANDROID_NDK path." >&2
    exit 1
fi

cmake ../ \
    -B./build\
    -DANDROID_ABI=armeabi-v7a \
    -DCMAKE_BUILD_TYPE=Release \
    -DANDROID_NDK=${ANDROID_NDK} \
    -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake \
    -DANDROID_PLATFORM=android-16  \
    -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=. $@

cd build && make -j8