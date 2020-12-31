@echo off

mkdir .\build
pushd .\build
cl /Fe"./stream.exe" /MDd ..\src\*.cpp /link /LTCG ..\lib\raylib.lib user32.lib gdi32.lib winmm.lib shell32.lib /NODEFAULTLIB:MSVCRT.lib
popd
