#ifndef SDL_PLATFORM_H
#define SDL_PLATFORM_H

#include <windows.h>
#include "game.h"

// DLL Functions
typedef void (*game_init_func) (Game_Data *game, Game_State *state);
typedef void (*game_refresh_func) (Game_Data *game, Game_State *state);
typedef void (*game_handle_input_func) ();
typedef void (*game_update_and_render_func) ();
typedef void (*game_sound_and_debug_func) ();

// Stub functions
void game_init_stub(Game_Data *it, Game_State *state){}
void game_refresh_stub(Game_Data *game, Game_State *state){}
void game_handle_input_stub(){}
void game_update_and_render_stub(){}
void game_sound_and_debug_stub(){}

struct SDL_Game_Code {
    void *game_code_dll;

    FILETIME dll_file_time;
    const char *source_dll_name;
    const char *temp_dll_name;

    game_init_func game_init;
    game_refresh_func game_refresh;
    game_handle_input_func game_handle_input;
    game_update_and_render_func game_update_and_render;
    game_sound_and_debug_func game_sound_and_debug;

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
