@echo off

pushd build\
if exist main.exe (
main.exe
) else (
echo [EXECUTE] : main.exe not found. Rebuilding Project.
..\src\build.bat
main.exe
)

popd