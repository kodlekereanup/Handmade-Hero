@echo off

pushd build\
::vcvars64
cl /FC /Zi ..\src\win32_handmade.c /Femain.exe User32.lib Gdi32.lib Dsound.lib

if %errorlevel% == 0 (
echo [BUILD SYSTEM] : Successfully Compiled
) else (
echo [BUILD SYSTEM] : Compilation Failed
)

popd

:: -Werror -Wall -pedantic
:: -Wextra 
:: w:/
:: build and handmade
:: handmade: code misc
:: code --> win32_handmade.c and build.bat
:: build: debug files