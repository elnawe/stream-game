@echo off

set CommonCompilerFlags=-diagnostics:column -WL -Od -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -FC -Z7 -GS- -Gs9999999
set CommonCompilerOptions=/DGAME_INTERNAL#1 /DGAME_DEVELOP#1

REM -W4

set ExecutableName="stream.exe"

IF NOT EXIST .\build mkdir .\build
pushd .\build

del *.pdb > NUL 2> NUL

REM Compile game.cpp into game.dll

cl %CommonCompilerOptions% /DDEBUG_MODE#1 %CommonCompilerFlags% /I..\include ..\src\game.cpp -LD /link -incremental:no -opt:ref -PDB:stream_%random%.pdb ..\lib\SDL2.lib ..\lib\SDL2main.lib ..\lib\SDL2_image.lib ..\lib\SDL2_mixer.lib ..\lib\SDL2_ttf.lib -EXPORT:GAME_INIT -EXPORT:GAME_REFRESH -EXPORT:GAME_HANDLE_INPUT -EXPORT:GAME_UPDATE_AND_RENDER -EXPORT:GAME_SOUND_AND_DEBUG


REM Compile the platform layer if needed (when stream.exe -or
REM %ExecutableName% doesn't exists)

REM IF NOT EXIST %ExecutableName% (
    cl %CommonCompilerOptions% /DDEBUG_MODE#1 %CommonCompilerFlags% /I..\include /Fe%ExecutableName% -MTd ..\src\sdl_platform.cpp /link -incremental:no -opt:ref ..\lib\SDL2.lib ..\lib\SDL2main.lib ..\lib\SDL2_mixer.lib ..\lib\SDL2_image.lib ..\lib\SDL2_ttf.lib
REM )

popd
