#ifndef WIN32_FTW_H
#define WIN32_FTW_H

#include <windows.h>
#include "ftw.h"

// DLL Functions
/* typedef void (*GAME_UPDATE_AND_RENDER_FUNC) (Game_Memory *memory, */
/*                                              Game_Input *input, */
/*                                              Game_Data *game); */

// Stub functions
/* void GAME_UPDATE_AND_RENDER_STUB(Game_Memory *m, */
/*                                  Game_Input *i, */
/*                                  Game_Data *g){} */


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

struct Memory {
    // NOTE: basically game_state, but will retain memory every
    // time we want to reload the DLL
    Game_State game_state;
};

#endif
