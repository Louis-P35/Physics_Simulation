#!/usr/bin/env bash

set -e  # Exit on error

# Go to the project root directory
cd "$(dirname "$0")/.."

# Create build directory if it does not exist
[ ! -d "build" ] && mkdir build

cd build

# Run CMake configuration
cmake .. -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release

# Build the project using Make
make -j$(nproc)
