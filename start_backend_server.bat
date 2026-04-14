@echo off
setlocal

echo ========================================================
echo  Turbomachinery Solver - Backend Server Automator
echo ========================================================
echo.

:: ---------------------------------------------------------------
:: 1. Define paths relative to the location of this batch file
::    %~dp0 always expands to the directory containing this script
::    so paths are correct regardless of the current working directory.
:: ---------------------------------------------------------------
set "SCRIPT_DIR=%~dp0"
set "VENV_PYTHON=%SCRIPT_DIR%.venv\Scripts\python.exe"
set "SYS_PYTHON=python"
set "BACKEND_DIR=%SCRIPT_DIR%Backend"
set "API_SCRIPT=%BACKEND_DIR%\api_server.py"

:: ---------------------------------------------------------------
:: 2. Verify the api_server script exists
:: ---------------------------------------------------------------
if not exist "%API_SCRIPT%" (
    echo [ERROR] Cannot find api_server.py at: %API_SCRIPT%
    echo         Please check your installation.
    pause
    exit /b 1
)

:: ---------------------------------------------------------------
:: 3. Guard: if port 8080 is already in use, skip launching again
:: ---------------------------------------------------------------
netstat -ano | findstr ":8080 " | findstr "LISTENING" >nul 2>&1
if %ERRORLEVEL% == 0 (
    echo [INFO] Port 8080 is already in use. Backend may already be running.
    exit /b 0
)

:: ---------------------------------------------------------------
:: 4. Pick the right Python executable
:: ---------------------------------------------------------------
if exist "%VENV_PYTHON%" (
    set "PYTHON_EXE=%VENV_PYTHON%"
    echo [INFO] Using virtual-env Python: %VENV_PYTHON%
) else (
    set "PYTHON_EXE=%SYS_PYTHON%"
    echo [WARN] .venv not found - falling back to system Python.
)

:: ---------------------------------------------------------------
:: 5. Launch the FastAPI server in its own minimised window.
::    We pass the FULL path to api_server.py as an argument so
::    Python does not depend on CWD.  The sys.path fix inside
::    api_server.py ensures modules (solver/, services/) resolve
::    correctly from that absolute path.
:: ---------------------------------------------------------------
echo [INFO] Starting Python backend server...
start /min "Turbomachinery API Backend" "%PYTHON_EXE%" "%API_SCRIPT%"

:: ---------------------------------------------------------------
:: 6. Brief pause to allow the FastAPI server to bind to port 8080,
::    then exit.  The C++ GUI polls /login every second anyway.
:: ---------------------------------------------------------------
timeout /t 2 /nobreak >nul

echo [SUCCESS] Backend launch initiated. GUI health-check will confirm readiness.
echo.
echo (To terminate the backend later, close the minimized command window.)
exit /b 0
