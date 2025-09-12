#!/bin/sh

# export NDK_HOME="/opt/android-sdk/ndk/29.0.14033849"

abi="arm64-v8a"
api_level="26"

mkdir -p out && cd out

cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=$NDK_HOME/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=$abi \
    -DANDROID_NATIVE_API_LEVEL=$api_level

cmake --build .

mv vbmeta_tool ../
cd .. && rm -rf out
