#ifndef WIN32_FTW_H
#define WIN32_FTW_H

#include <windows.h>
#include "ftw.h"

struct SDL_Game_Code {
    void *game_code_dll;

    FILETIME dll_file_time;
    const char *source_dll_name;
    const char *temp_dll_name;

    Game_Update_And_Render *game_update_and_render;

    bool is_valid;
};

struct SDL_Game_Config {
    FILETIME last_update;
    bool is_valid;
};

#endif
