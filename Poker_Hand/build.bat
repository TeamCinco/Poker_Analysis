@echo off
echo ==========================================
echo High-Performance Poker Hand Analyzer
echo Build Script for Windows
echo ==========================================
echo.

REM Check if CMake is available
cmake --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: CMake is not installed or not in PATH
    echo Please install CMake from https://cmake.org/download/
    pause
    exit /b 1
)

REM Create build directory
if not exist build mkdir build
cd build

echo Configuring with CMake...
cmake .. -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 16 2019" -A x64

if errorlevel 1 (
    echo.
    echo Trying with different generator...
    cmake .. -DCMAKE_BUILD_TYPE=Release -G "MinGW Makefiles"
    
    if errorlevel 1 (
        echo.
        echo ERROR: CMake configuration failed
        echo Please check your build environment
        pause
        exit /b 1
    )
)

echo.
echo Building project...
cmake --build . --config Release --parallel

if errorlevel 1 (
    echo.
    echo ERROR: Build failed
    echo Please check the error messages above
    pause
    exit /b 1
)

echo.
echo ==========================================
echo Build completed successfully!
echo ==========================================
echo.
echo To run the poker analyzer:
echo   cd build
echo   poker_analysis_generator demo
echo   poker_analysis_generator perf  
echo   poker_analysis_generator data
echo.
echo Generated files will be in the output/ directory
echo.
pause
