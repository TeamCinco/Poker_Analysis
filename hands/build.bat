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
    echo Original system: build\Release\monte_carlo_poker.exe
    if exist "Release\dynamic_poker_analyzer.exe" (
        echo Dynamic system: build\Release\dynamic_poker_analyzer.exe
    )
    echo.
    echo === ORIGINAL SYSTEM ===
    echo   cd build\Release
    echo   monte_carlo_poker.exe          ^(analyze all table sizes^)
    echo   monte_carlo_poker.exe 6        ^(6-max only^)
    echo.
    echo === NEW DYNAMIC BOUNDARY-DRIVEN SYSTEM ===
    echo   dynamic_poker_analyzer.exe                ^(full boundary exploration^)
    echo   dynamic_poker_analyzer.exe 6              ^(6-max boundary exploration^)
    echo   dynamic_poker_analyzer.exe --fast         ^(fast mode - reduced simulations^)
    echo   dynamic_poker_analyzer.exe 6 --fast       ^(6-max fast mode^)
    echo   dynamic_poker_analyzer.exe --help         ^(show detailed help^)
    echo.
    echo === KEY DIFFERENCES ===
    echo Original: Predefined scenario lists ^(traditional approach^)
    echo Dynamic:  Boundary parameters + pure Monte Carlo exploration
    echo.
) else if exist "monte_carlo_poker.exe" (
    echo.
    echo === Build Successful! ===
    echo Original system: build\monte_carlo_poker.exe
    if exist "dynamic_poker_analyzer.exe" (
        echo Dynamic system: build\dynamic_poker_analyzer.exe
    )
    echo.
    echo === ORIGINAL SYSTEM ===
    echo   cd build
    echo   monte_carlo_poker.exe          ^(analyze all table sizes^)
    echo   monte_carlo_poker.exe 6        ^(6-max only^)
    echo.
    echo === NEW DYNAMIC BOUNDARY-DRIVEN SYSTEM ===
    echo   dynamic_poker_analyzer.exe                ^(full boundary exploration^)
    echo   dynamic_poker_analyzer.exe 6              ^(6-max boundary exploration^)
    echo   dynamic_poker_analyzer.exe --fast         ^(fast mode^)
    echo   dynamic_poker_analyzer.exe --help         ^(detailed help^)
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
