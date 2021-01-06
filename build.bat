@echo off

set CommonCompilerFlags=-diagnostics:column -WL -Od -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -FC -Z7 -GS- -Gs9999999

set ExecutableName="stream.exe"

IF NOT EXIST .\build mkdir .\build
pushd .\build

del *.pdb > NUL 2> NUL

REM Compile game.cpp into game.dll

cl %CommonCompilerFlags% /I..\include ..\src\game.cpp -LD /link -incremental:no -opt:ref -PDB:stream_%random%.pdb ..\lib\SDL2.lib ..\lib\SDL2main.lib -EXPORT:game_init -EXPORT:game_loop

REM Compile the platform layer if needed (when stream.exe -or
REM %ExecutableName% doesn't exists)

REM IF NOT EXIST %ExecutableName% (
   cl %CommonCompilerFlags% /I..\include /Fe%ExecutableName% -MTd ..\src\sdl_platform.cpp /link -incremental:no -opt:ref ..\lib\SDL2.lib ..\lib\SDL2main.lib
REM )

popd
