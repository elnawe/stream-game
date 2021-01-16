#ifndef GAME_H
#define GAME_H

#include "entity.h"
#include "item.h"
#include "SDL.h"
#include "texture_manager.h"

struct Game_Memory {
    bool is_initialized;

    u64 permanent_storage_size;
    void *permanent_storage; // NOTE: Required to be zero-ed out at startup

    u64 transient_storage_size;
    void *transient_storage; // NOTE: Required to be zero-ed out at startup
};

////////
////////
enum Game_Scene {
                 MENU,
                 GAME,
                 PAUSE
};

struct Game_State {
    const Uint8 *input;
    Vector2 player_position;
};

struct Game_Options {
    r32 music_volume;
};

struct Game_Data {
    float screen_width;
    float screen_height;
    float window_width;
    float window_height;
    bool running;
    float delta_time;
    SDL_Renderer *renderer;
    SDL_Window *window;

    Entity_List entity_list;
    Item_List item_list;
    Texture_Manager texture_manager;
    Game_Options options;
};

void GAME_INIT(Game_Data *g, Game_State *state);
void GAME_REFRESH(Game_Data *g, Game_State *state);
void GAME_HANDLE_INPUT();
void GAME_UPDATE_AND_RENDER(Game_Memory *memory);
void GAME_SOUND_AND_DEBUG();

#endif
