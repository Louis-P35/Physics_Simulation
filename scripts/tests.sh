#!/usr/bin/env bash

set -e

cd "$(dirname "$0")/.."

# Ensure tests are built
./scripts/build_release.sh

# Run the tests using CTest (assuming tests are defined)
cd build/tests
ctest --output-on-failure
