@echo off

mkdir .\build
pushd .\build
cl /MDd ..\src\main.cpp /link /LTCG ..\lib\raylib.lib user32.lib gdi32.lib winmm.lib shell32.lib
popd
