<#
.SYNOPSIS
Orchestrates the build process using MinGW and Ninja to match the user's Qt installation.
#>

$ErrorActionPreference = "Stop"
$WorkingDir = Get-Location

Write-Host "=========================================="
Write-Host "1. PRE-REQUISITE PATH DEFINITIONS"
Write-Host "=========================================="

# Define Qt and MinGW paths
$QT_ROOT = "D:\Qt\6.10.2\mingw_64"
$MINGW_BIN = "D:\Qt\Tools\mingw1310_64\bin"
$NINJA_EXE = "D:\Qt\Tools\Ninja\ninja.exe"

# Try different Inno Setup locations (System and User)
$ISCC_PATHS = @(
    "C:\Program Files (x86)\Inno Setup 6\ISCC.exe",
    "C:\Program Files\Inno Setup 6\ISCC.exe",
    "$env:LocalAppData\Programs\Inno Setup 6\ISCC.exe"
)

$ISCC_EXE = ""
foreach ($path in $ISCC_PATHS) {
    if (Test-Path $path) {
        $ISCC_EXE = $path
        break
    }
}

# Add MinGW to PATH for this session so compilers and DLLs are found
$env:PATH = "$MINGW_BIN;$env:PATH"

Write-Host "Using MinGW: $MINGW_BIN"
Write-Host "Using Qt: $QT_ROOT"

Write-Host "=========================================="
Write-Host "2. INNO SETUP CHECK"
Write-Host "=========================================="
if ($ISCC_EXE -eq "") {
    Write-Host "Inno Setup not found in common paths. Attempting install via winget..."
    winget install -e --id JRSoftware.InnoSetup --accept-package-agreements --accept-source-agreements
    # Check again after install
    foreach ($path in $ISCC_PATHS) {
        if (Test-Path $path) { $ISCC_EXE = $path; break }
    }
}

if ($ISCC_EXE -eq "") {
    Write-Error "Inno Setup (ISCC.exe) still not found. Please ensure it is installed and available."
    exit 1
}
Write-Host "Using ISCC: $ISCC_EXE"

# Locate Python (prefer .venv)
$VENV_PYTHON = ".\\.venv\\Scripts\\python.exe"
if (Test-Path $VENV_PYTHON) {
    $PY = Resolve-Path $VENV_PYTHON
    Write-Host "Using venv Python: $PY"
} else {
    $PY = "python"
    Write-Host "Warning: .venv not found, using system Python."
}

Write-Host "=========================================="
Write-Host "3. BUILD PYTHON BACKEND (PyInstaller)"
Write-Host "=========================================="
& $PY -m pip install pyinstaller --quiet

Push-Location ".\Backend"
& $PY -m PyInstaller api_server.spec --clean --noconfirm
Pop-Location

$BACKEND_EXE = ".\Backend\dist\backend_server.exe"
if (-not (Test-Path $BACKEND_EXE)) {
    Write-Error "PyInstaller FAILED."
    exit 1
}
Write-Host "[OK] Backend built: $BACKEND_EXE"

Write-Host "=========================================="
Write-Host "4. C++ UI BUILD (MinGW + Ninja)"
Write-Host "=========================================="
if (Test-Path ".\ReleaseBuild") { Remove-Item -Recurse -Force ".\ReleaseBuild" }

# Run CMake using Ninja and MinGW compilers
# We point explicitly to the compilers and the Qt6 CMake files
cmake -G "Ninja" -B ReleaseBuild -S . `
    -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_MAKE_PROGRAM="$NINJA_EXE" `
    -DCMAKE_C_COMPILER="$MINGW_BIN/gcc.exe" `
    -DCMAKE_CXX_COMPILER="$MINGW_BIN/g++.exe" `
    -DQt6_DIR="$QT_ROOT/lib/cmake/Qt6"

# Build target
cmake --build ReleaseBuild --target Turbomachines_GUI

# Copy backend to GUI folder for local testing
if (-not (Test-Path ".\ReleaseBuild\bin")) { New-Item -ItemType Directory -Path ".\ReleaseBuild\bin" -Force }
Copy-Item ".\Backend\dist\backend_server.exe" -Destination ".\ReleaseBuild\bin\backend_server.exe" -Force
Write-Host "[OK] Backend copied to bin for testing: ReleaseBuild\bin\backend_server.exe"

# Run windeployqt
Write-Host "Collecting Qt Dependencies via windeployqt..."
$GUI_EXE = ".\ReleaseBuild\bin\Turbomachines_GUI.exe"
$WINDEPLOYQT = "$QT_ROOT\bin\windeployqt.exe"
& $WINDEPLOYQT --release --no-translations --compiler-runtime $GUI_EXE

Write-Host "=========================================="
Write-Host "5. INNO SETUP COMPILATION"
Write-Host "=========================================="
& "$ISCC_EXE" .\installer.iss

Write-Host "=========================================="
Write-Host "BUILD COMPLETE!"
Write-Host "Installer: .\installer\TurbomachinerySolver_Setup.exe"
Write-Host "=========================================="
