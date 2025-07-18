#!/bin/bash

echo "Building Poker Analytics System for Unix-like systems..."
echo

# Detect platform
if [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="macOS"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    PLATFORM="Linux"
else
    PLATFORM="Unix"
fi

echo "Detected platform: $PLATFORM"
echo

# Check if CMake is installed
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake is not installed. Please install CMake first."
    echo "  On macOS: brew install cmake"
    echo "  On Ubuntu/Debian: sudo apt-get install cmake"
    echo "  On Red Hat/CentOS: sudo yum install cmake"
    exit 1
fi

# Check if a C++ compiler is available
if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo "Error: No C++ compiler found. Please install a C++ compiler."
    echo "  On macOS: Install Xcode Command Line Tools"
    echo "  On Ubuntu/Debian: sudo apt-get install build-essential"
    echo "  On Red Hat/CentOS: sudo yum groupinstall 'Development Tools'"
    exit 1
fi

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
if [[ "$PLATFORM" == "macOS" ]]; then
    # Use Xcode generator on macOS if available, otherwise use Unix Makefiles
    if command -v xcodebuild &> /dev/null; then
        cmake .. -G "Xcode"
    else
        cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
    fi
else
    # Use Unix Makefiles for Linux
    cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
fi

# Check if configuration was successful
if [ $? -ne 0 ]; then
    echo "CMake configuration failed. Please check error messages above."
    exit 1
fi

# Build the project
echo "Building project..."
cmake --build . --config Release

# Check if build was successful
if [ $? -ne 0 ]; then
    echo "Build failed. Please check error messages above."
    exit 1
fi

echo
echo "Build completed successfully!"

# Find the executable
if [ -f "bin/poker_analytics" ]; then
    EXECUTABLE="bin/poker_analytics"
elif [ -f "poker_analytics" ]; then
    EXECUTABLE="poker_analytics"
elif [ -f "Release/poker_analytics" ]; then
    EXECUTABLE="Release/poker_analytics"
else
    echo "Warning: Could not find executable. Please check the build output."
    EXECUTABLE=""
fi

if [ -n "$EXECUTABLE" ]; then
    echo "Executable location: build/$EXECUTABLE"
    echo

    # Ask user if they want to run the program
    read -p "Run the program now? (y/n): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "Running poker_analytics..."
        ./$EXECUTABLE
    fi
else
    echo "Please locate the executable manually in the build directory."
fi

echo
echo "Build script completed."
