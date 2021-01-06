#ifndef GAME_H
#define GAME_H

#include "SDL.h"
#include "entity.h"

// NOTE: Use this!!!11!
struct Game_Data {
    int screen_width;
    int screen_height;
    SDL_Renderer *renderer;
    //Entity[10000] entities;
};

void game_init(Game_Data *game);
void game_loop(Game_Data *game);

#endif
