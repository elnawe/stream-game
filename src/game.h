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
    Game_State state;
    Game_Options options;
};

void GAME_INIT(Game_Data *game);
void GAME_HANDLE_INPUT(Game_Data *game);
void GAME_UPDATE_AND_RENDER(Game_Data *game);
void GAME_SOUND_AND_DEBUG(Game_Data *game);

#endif
