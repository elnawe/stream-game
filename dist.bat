@echo off

set ExecutableName="play.exe"

IF NOT EXIST .\dist mkdir .\dist
pushd .\dist

del *.pdb > NUL 2> NUL
del *.dll > NUL 2> NUL
del *.exe > NUL 2> NUL

cl /I..\include ..\src\game.cpp -LD /link -incremental:no -opt:ref -PDB:stream_%random%.pdb ..\lib\SDL2.lib ..\lib\SDL2main.lib ..\lib\SDL2_image.lib ..\lib\SDL2_mixer.lib -EXPORT:GAME_INIT -EXPORT:GAME_HANDLE_INPUT -EXPORT:GAME_UPDATE_AND_RENDER -EXPORT:GAME_SOUND_AND_DEBUG

cl /I..\include /Fe%ExecutableName% -MTd ..\src\sdl_platform.cpp /link -incremental:no -opt:ref ..\lib\SDL2.lib ..\lib\SDL2main.lib ..\lib\SDL2_mixer.lib ..\lib\SDL2_image.lib ..\lib\SDL2_ttf.lib

del *.pdb > NUL 2> NUL
del *.obj > NUL 2> NUL

copy ..\lib\SDL2.dll .\
copy ..\lib\SDL2_*.dll .\
copy ..\lib\libfreetype-6.dll .\
copy ..\lib\zlib1.dll .\
copy ..\lib\libpng16-16.dll .\
copy ..\lib\libmpg123-0.dll .\

IF NOT EXIST .\assets mkdir .\assets
del .\assets\* /Q
copy ..\assets\* .\assets\

popd
