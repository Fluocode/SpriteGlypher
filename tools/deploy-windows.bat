@echo off
setlocal EnableExtensions
REM Despliega DLLs de Qt y runtime de MinGW junto a SpriteGlypher.exe (equivalente a windeployqt del .pro).
REM Uso:   deploy-windows.bat ruta\completa\SpriteGlypher.exe
REM Requiere windeployqt en PATH, o define QTDIR (ej. C:\Qt\6.11.0\mingw_64) o WINDEPLOYQT=ruta\a\windeployqt.exe

if "%~1"=="" (
    echo Uso: %~nx0 ^<ruta\SpriteGlypher.exe^>
    exit /b 1
)

if not exist "%~1" (
    echo No existe: %~1
    exit /b 1
)

set "WQ="
if defined WINDEPLOYQT set "WQ=%WINDEPLOYQT%"
if not defined WQ if defined QTDIR set "WQ=%QTDIR%\bin\windeployqt.exe"
if not defined WQ set "WQ=windeployqt.exe"

echo Ejecutando: "%WQ%" "%~1" --compiler-runtime
"%WQ%" "%~1" --compiler-runtime
exit /b %ERRORLEVEL%
