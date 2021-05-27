@echo off

pushd build\
if exist main.exe (
del main.exe
echo [CLEAN UP]: main.exe cleaned 
) else (
echo [CLEAN UP]: main.exe not found
)

if exist win32_handmade.obj (
del win32_handmade.obj
echo [CLEAN UP]: win32_handmande.obj cleaned 
) else (
echo [CLEAN UP]: win32_handmade.obj not found
)

if exist main.ilk (
del main.ilk
echo [CLEAN UP]: main.ilk cleaned 
) else (
echo [CLEAN UP]: main.ilk not found
)

if exist main.pdb (
del main.pdb
echo [CLEAN UP]: main.pdb cleaned 
) else (
echo [CLEAN UP]: main.pdb not found
)

if exist vc140.pdb (
del vc140.pdb
echo [CLEAN UP]: vc140.pdb cleaned 
) else (
echo [CLEAN UP]: vc140.pdb not found
)

popd