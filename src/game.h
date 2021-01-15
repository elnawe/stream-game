#ifndef GAME_H
#define GAME_H

#include "entity.h"
#include "item.h"
#include "SDL.h"
#include "texture_manager.h"

enum Game_Scene {
                 MENU,
                 GAME,
                 PAUSE
};

struct Game_State {
    const Uint8 *input;
    Game_Scene scene;
    Vector2 player_position;

    bool is_initialized;
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
void GAME_UPDATE_AND_RENDER();
void GAME_SOUND_AND_DEBUG();

#endif
