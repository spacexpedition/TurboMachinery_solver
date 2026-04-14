<#
.SYNOPSIS
Orchestrates the build process: C++ compilation, Python PyInstaller serialization, Qt linking, and Inno Setup packager.
#>

$ErrorActionPreference = "Stop"
$WorkingDir = Get-Location

Write-Host "=========================================="
Write-Host "1. PRE-REQUISITE CHECKS"
Write-Host "=========================================="
# Check if Inno Setup is installed, if not, download it via winget
if (-not (Get-Command "ISCC" -ErrorAction SilentlyContinue)) {
    Write-Host "Inno Setup Compiler (ISCC) not found in PATH."
    Write-Host "Attempting automatic installation of Inno Setup via winget..."
    winget install -e --id JRSoftware.InnoSetup --accept-package-agreements --accept-source-agreements
    
    # Reload environment block
    $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
}

Write-Host "=========================================="
Write-Host "2. CLOUD BACKEND (Skipped for Desktop Build)"
Write-Host "=========================================="
Write-Host "Backend is now hosted on Google Cloud Run. Desktop app is standalone!"

Write-Host "=========================================="
Write-Host "3. C++ UI BUILD & DEPLOYMENT"
Write-Host "=========================================="
# Clean up previous release if present
if (Test-Path ".\ReleaseBuild") { Remove-Item -Recurse -Force ".\ReleaseBuild" }

# Run CMake build pipeline
cmake -B ReleaseBuild -S . -DCMAKE_BUILD_TYPE=Release
cmake --build ReleaseBuild --config Release

# Run windeployqt to grab UI DLLs automatically
Write-Host "Collecting Qt Dependencies via windeployqt..."
$GUI_EXE = ".\ReleaseBuild\bin\Turbomachines_GUI.exe"
windeployqt $GUI_EXE

Write-Host "=========================================="
Write-Host "5. INNO SETUP COMPILATION"
Write-Host "=========================================="
# Compile the installer executable
$ISCC_PATH = "ISCC"
if (Test-Path "${env:ProgramFiles(x86)}\Inno Setup 6\ISCC.exe") {
    $ISCC_PATH = "& `"${env:ProgramFiles(x86)}\Inno Setup 6\ISCC.exe`""
}
Invoke-Expression "$ISCC_PATH .\installer.iss"

Write-Host "=========================================="
Write-Host "BUILD COMPLETE!"
Write-Host "Installer is available at: .\installer\TurbomachinerySolver_Setup.exe"
Write-Host "=========================================="
