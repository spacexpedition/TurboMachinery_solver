@echo off
setlocal

echo ========================================================
echo Turbomachinery Solver - Backend Server Automator
echo ========================================================
echo.

:: 1. Define paths relative to this batch file
set "VENV_DIR=%~dp0.venv"
set "BACKEND_DIR=%~dp0Backend"

:: 2. Start the Backend API in a separate, minimized background window
echo [INFO] Starting Python backend server...
if exist "%VENV_DIR%\Scripts\python.exe" (
    start /min "Turbomachinery API Backend" cmd /c "cd /d "%BACKEND_DIR%" && "%VENV_DIR%\Scripts\python.exe" api_server.py"
) else (
    start /min "Turbomachinery API Backend" cmd /c "cd /d "%BACKEND_DIR%" && python api_server.py"
)

:: 3. Wait briefly to allow the FastAPI server to bind to port 8080
timeout /t 3 /nobreak > nul

echo [SUCCESS] Backend is now running in the background.
echo.
echo ========================================================
echo You can now safely run the C++ GUI from your IDE
echo (or double-click the GUI executable). It will connect
echo successfully without throwing "Connection Error".
echo ========================================================
echo.
echo (To terminate the backend later, simply close the minimized command window).
pause
