#!/bin/bash
set -e

# Compiler and flags
CXX=g++
CXXFLAGS="-std=c++17 -Wall -Wextra -O2"

# Build server
echo "Compiling tcp_server.cpp..."
$CXX $CXXFLAGS tcp_server.cpp -o tcp_server

# Build client
echo "Compiling tcp_client.cpp..."
$CXX $CXXFLAGS tcp_client.cpp -o tcp_client

echo "Build complete!"
