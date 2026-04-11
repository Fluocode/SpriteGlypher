@echo off
setlocal

REM Deploy Qt 6 (MinGW 64-bit) runtime next to SpriteGlypher.exe.
REM With Qt 6, a Release build from qmake/Makefile already runs windeployqt (see SpriteGlypher.pro).
REM Use this script only if you need to re-deploy without rebuilding, or your build system skipped POST_LINK.
REM
REM Usage:
REM   windeploy-qt6-mingw.bat "C:\full\path\to\build-...\release\SpriteGlypher.exe"
REM
REM If your Qt install is not the default below, either edit QT_MINGW_BIN or set it before calling:
REM   set QT_MINGW_BIN=C:\Qt\6.11.0\mingw_64\bin
REM   windeploy-qt6-mingw.bat "...\SpriteGlypher.exe"

if "%~1"=="" (
    echo Usage: %~nx0 "path\to\SpriteGlypher.exe"
    exit /b 1
)

if not exist "%~1" (
    echo File not found: %~1
    exit /b 1
)

if "%QT_MINGW_BIN%"=="" set "QT_MINGW_BIN=C:\Qt\6.11.0\mingw_64\bin"

if not exist "%QT_MINGW_BIN%\windeployqt.exe" (
    echo Could not find windeployqt.exe under:
    echo   %QT_MINGW_BIN%
    echo Set QT_MINGW_BIN to your kit's bin folder, e.g. C:\Qt\6.x.x\mingw_64\bin
    exit /b 1
)

pushd "%~dp1"
"%QT_MINGW_BIN%\windeployqt.exe" "%~nx1"
set ERR=%ERRORLEVEL%
popd

if not %ERR%==0 (
    echo windeployqt exited with code %ERR%
    exit /b %ERR%
)

echo.
echo Done. You can run SpriteGlypher.exe from: %~dp1
