#!/bin/bash

# Surgical Robotics Safety Platform - Build Script
echo "Building Surgical Robotics Safety Platform..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring project with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "Building project..."
make -j$(nproc)

# Run tests
echo "Running tests..."
make test

# Build documentation (if doxygen is available)
if command -v doxygen &> /dev/null; then
    echo "Building documentation..."
    doxygen ../Doxyfile
fi

echo "Build complete!"
echo "Executables are in: build/bin/"
echo "To start the dashboard: ./scripts/start_dashboard.sh"
