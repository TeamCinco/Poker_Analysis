@echo off
echo Building Poker Analytics System for Windows...
echo.

REM Create build directory
if not exist "build" mkdir build
cd build

REM Configure with CMake
echo Configuring with CMake...
cmake .. -G "Visual Studio 17 2022" -A x64

REM Check if configuration was successful
if %errorlevel% neq 0 (
    echo CMake configuration failed. Trying with different generator...
    cmake .. -G "MinGW Makefiles"
    if %errorlevel% neq 0 (
        echo CMake configuration failed with all generators. Please check CMake installation.
        pause
        exit /b 1
    )
)

REM Build the project
echo Building project...
cmake --build . --config Release

REM Check if build was successful
if %errorlevel% neq 0 (
    echo Build failed. Please check error messages above.
    pause
    exit /b 1
)

echo.
echo Build completed successfully!
echo Executable location: build\bin\Release\poker_analytics.exe (or build\bin\poker_analytics.exe)
echo.

REM Ask user if they want to run the program
set /p run_program=Run the program now? (y/n): 
if /i "%run_program%"=="y" (
    if exist "bin\Release\poker_analytics.exe" (
        echo Running poker_analytics.exe...
        bin\Release\poker_analytics.exe
    ) else if exist "bin\poker_analytics.exe" (
        echo Running poker_analytics.exe...
        bin\poker_analytics.exe
    ) else (
        echo Could not find executable. Please check the build output.
    )
)

pause
