# PowerShell build script for Poker Analytics System on Windows
Write-Host "Building Poker Analytics System for Windows..." -ForegroundColor Cyan
Write-Host ""

# Create build directory
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

Set-Location "build"

# Configure with CMake
Write-Host "Configuring with CMake..." -ForegroundColor Yellow

# Try Visual Studio generator first
$configSuccess = $false
try {
    & cmake .. -G "Visual Studio 17 2022" -A x64 2>&1 | Out-Host
    if ($LASTEXITCODE -eq 0) {
        $configSuccess = $true
        Write-Host "Configuration successful with Visual Studio 2022" -ForegroundColor Green
    }
} catch {
    Write-Host "Visual Studio 2022 not found, trying Visual Studio 2019..." -ForegroundColor Yellow
}

# Try Visual Studio 2019 if 2022 failed
if (-not $configSuccess) {
    try {
        & cmake .. -G "Visual Studio 16 2019" -A x64 2>&1 | Out-Host
        if ($LASTEXITCODE -eq 0) {
            $configSuccess = $true
            Write-Host "Configuration successful with Visual Studio 2019" -ForegroundColor Green
        }
    } catch {
        Write-Host "Visual Studio 2019 not found, trying MinGW..." -ForegroundColor Yellow
    }
}

# Try MinGW Makefiles as fallback
if (-not $configSuccess) {
    try {
        & cmake .. -G "MinGW Makefiles" 2>&1 | Out-Host
        if ($LASTEXITCODE -eq 0) {
            $configSuccess = $true
            Write-Host "Configuration successful with MinGW" -ForegroundColor Green
        }
    } catch {
        Write-Host "MinGW not found, trying Unix Makefiles..." -ForegroundColor Yellow
    }
}

# Try Unix Makefiles as final fallback
if (-not $configSuccess) {
    try {
        & cmake .. -G "Unix Makefiles" 2>&1 | Out-Host
        if ($LASTEXITCODE -eq 0) {
            $configSuccess = $true
            Write-Host "Configuration successful with Unix Makefiles" -ForegroundColor Green
        }
    } catch {
        Write-Host "All generators failed" -ForegroundColor Red
    }
}

# Check if configuration was successful
if (-not $configSuccess) {
    Write-Host "ERROR: CMake configuration failed with all generators." -ForegroundColor Red
    Write-Host "Please check that CMake and a C++ compiler are installed:" -ForegroundColor Red
    Write-Host "  - Install Visual Studio 2019/2022 with C++ workload" -ForegroundColor Yellow
    Write-Host "  - Or install MinGW-w64" -ForegroundColor Yellow
    Write-Host "  - Make sure CMake is in your PATH" -ForegroundColor Yellow
    Read-Host "Press Enter to exit..."
    Set-Location ".."
    exit 1
}

# Build the project
Write-Host ""
Write-Host "Building project..." -ForegroundColor Yellow
& cmake --build . --config Release 2>&1 | Out-Host

# Check if build was successful
if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "ERROR: Build failed. Please check error messages above." -ForegroundColor Red
    Read-Host "Press Enter to exit..."
    Set-Location ".."
    exit 1
}

Write-Host ""
Write-Host "Build completed successfully!" -ForegroundColor Green

# Find the executable
$executable = $null
$possiblePaths = @(
    "bin\Release\poker_analytics.exe",
    "bin\poker_analytics.exe", 
    "Release\poker_analytics.exe",
    "poker_analytics.exe"
)

foreach ($path in $possiblePaths) {
    if (Test-Path $path) {
        $executable = $path
        break
    }
}

if ($executable) {
    Write-Host "Executable location: build\$executable" -ForegroundColor Green
    Write-Host ""
    
    # Ask user if they want to run the program
    $runProgram = Read-Host "Run the program now? (y/n)"
    if ($runProgram -eq 'y' -or $runProgram -eq 'Y') {
        Write-Host "Running poker_analytics..." -ForegroundColor Cyan
        & ".\$executable"
    }
} else {
    Write-Host "Warning: Could not find executable. Please check the build output." -ForegroundColor Yellow
    Write-Host "Look for poker_analytics.exe in the build directory." -ForegroundColor Yellow
}

Write-Host ""
Write-Host "Build script completed." -ForegroundColor Green
Set-Location ".."
