# Qt6 Setup Script for LogWatcher
# Run this script after installing Qt6 to configure the environment

Write-Host "=== Qt6 Setup Script for LogWatcher ===" -ForegroundColor Cyan
Write-Host ""

# Check if Qt6 directory exists
$qtPaths = @(
    "C:\Qt\6.5.0\msvc2022_64",
    "C:\Qt\6.5.1\msvc2022_64",
    "C:\Qt\6.5.2\msvc2022_64",
    "C:\Qt\6.5.3\msvc2022_64",
    "C:\Qt\6.6.0\msvc2022_64",
    "C:\Qt\6.6.1\msvc2022_64",
    "C:\Qt\6.6.2\msvc2022_64",
    "C:\Qt\6.6.3\msvc2022_64",
    "C:\Qt\6.7.0\msvc2022_64",
    "C:\Qt\6.7.1\msvc2022_64",
    "C:\Qt\6.7.2\msvc2022_64",
    "C:\Qt\6.8.0\msvc2022_64",
    "C:\Qt\6.8.1\msvc2022_64",
    "C:\Qt\6.8.2\msvc2022_64"
)

$foundQtPath = $null
foreach ($path in $qtPaths) {
    if (Test-Path $path) {
        $foundQtPath = $path
        break
    }
}

if ($foundQtPath) {
    Write-Host "Found Qt6 at: $foundQtPath" -ForegroundColor Green
    Write-Host ""
    
    # Set environment variable for current session
    $env:Qt6_DIR = $foundQtPath
    $env:CMAKE_PREFIX_PATH = $foundQtPath
    
    Write-Host "Environment variables set for current session:" -ForegroundColor Yellow
    Write-Host "  Qt6_DIR = $env:Qt6_DIR" -ForegroundColor White
    Write-Host "  CMAKE_PREFIX_PATH = $env:CMAKE_PREFIX_PATH" -ForegroundColor White
    Write-Host ""
    
    # Ask if user wants to set permanently
    $response = Read-Host "Do you want to set Qt6_DIR permanently? (Y/N)"
    if ($response -eq 'Y' -or $response -eq 'y') {
        [Environment]::SetEnvironmentVariable("Qt6_DIR", $foundQtPath, "User")
        Write-Host "Qt6_DIR set permanently for user." -ForegroundColor Green
    }
    
    Write-Host ""
    Write-Host "You can now build the project with:" -ForegroundColor Cyan
    Write-Host "  cmake -S . -B build -DCMAKE_PREFIX_PATH='$foundQtPath'" -ForegroundColor White
    Write-Host "  cmake --build build --config Release" -ForegroundColor White
} else {
    Write-Host "Qt6 not found in common locations." -ForegroundColor Red
    Write-Host ""
    Write-Host "Please install Qt6 first:" -ForegroundColor Yellow
    Write-Host "1. Download Qt Online Installer from: https://www.qt.io/download-qt-installer" -ForegroundColor White
    Write-Host "2. Install Qt 6.x with MSVC 2022 compiler" -ForegroundColor White
    Write-Host "3. Run this script again after installation" -ForegroundColor White
    Write-Host ""
    Write-Host "Or manually specify the path when running CMake:" -ForegroundColor Yellow
    Write-Host "  cmake -S . -B build -DCMAKE_PREFIX_PATH='C:/Path/To/Qt6/msvc2022_64'" -ForegroundColor White
}

Write-Host ""
Write-Host "Press any key to exit..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
