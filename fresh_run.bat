@echo off

pushd build\

..\clean.bat
..\build.bat
..\execute.bat

popd