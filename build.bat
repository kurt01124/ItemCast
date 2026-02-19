@echo off
chcp 65001 >nul
echo [ItemCast] Build started...

cmake -B build -G "Visual Studio 17 2022" -A x64
if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake configure failed
    pause
    exit /b 1
)

cmake --build build --config Release
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Build failed
    pause
    exit /b 1
)

echo [ItemCast] Build complete: build\bin\Release\ItemCast.exe
explorer "build\bin\Release"
