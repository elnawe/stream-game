@echo off

set CommonCompilerFlags=-diagnostics:column -WL -Od -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -FC -Z7 -GS- -Gs9999999

set CommonCompilerOptions=/DFTW_INTERNAL#1 /DFTW_DEVELOP#1

REM -W4

set ExecutableName="FTW.exe"

IF NOT EXIST .\build mkdir .\build
pushd .\build

del *.pdb > NUL 2> NUL

REM Compile ftw.cpp into ftw.dll

cl %CommonCompilerOptions% %CommonCompilerFlags% /I..\include ..\src\ftw.cpp -LD /link -incremental:no -opt:ref -PDB:stream_%random%.pdb ..\lib\SDL2.lib ..\lib\SDL2main.lib ..\lib\SDL2_image.lib ..\lib\SDL2_mixer.lib ..\lib\SDL2_ttf.lib -EXPORT:game_update_and_render


REM Compile the platform layer if needed (when stream.exe -or
REM %ExecutableName% doesn't exists)

REM IF NOT EXIST %ExecutableName% (
    cl %CommonCompilerOptions% %CommonCompilerFlags% /I..\include /Fe%ExecutableName% -MTd ..\src\win32_ftw.cpp /link -incremental:no -opt:ref ..\lib\SDL2.lib ..\lib\SDL2main.lib ..\lib\SDL2_mixer.lib ..\lib\SDL2_image.lib ..\lib\SDL2_ttf.lib
REM )

popd
