@echo off
echo === Building Monte Carlo Poker Analysis System ===

REM Create build directory
if not exist "build" mkdir build
cd build

REM Configure with CMake
echo Configuring with CMake...
cmake .. -DCMAKE_BUILD_TYPE=Release

REM Build the project
echo Building project...
cmake --build . --config Release

REM Check if build was successful
if exist "Release\monte_carlo_poker.exe" (
    echo.
    echo === Build Successful! ===
    echo Executable: build\Release\monte_carlo_poker.exe
    echo.
    echo To run analysis:
    echo   cd build\Release
    echo   monte_carlo_poker.exe
    echo.
    echo Or for specific table size:
    echo   monte_carlo_poker.exe 6
    echo   monte_carlo_poker.exe 9
    echo.
) else if exist "monte_carlo_poker.exe" (
    echo.
    echo === Build Successful! ===
    echo Executable: build\monte_carlo_poker.exe
    echo.
    echo To run analysis:
    echo   cd build
    echo   monte_carlo_poker.exe
    echo.
) else (
    echo.
    echo === Build Failed ===
    echo Check the error messages above.
    echo.
    pause
    exit /b 1
)

cd ..
echo Build complete!
pause
