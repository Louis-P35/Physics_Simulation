#!/usr/bin/env bash

set -e

cd "$(dirname "$0")/.."

# Assuming the main executable is named after the project
EXECUTABLE="CppTemplate"

# Run the executable from the build directory
./build/src/$EXECUTABLE
