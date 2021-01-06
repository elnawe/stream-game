#ifndef ENTITY_H
#define ENTITY_H

#include "SDL.h"
#include "math.h"

struct Entity {
    Vector2 position;
    SDL_Rect src_rect;
    SDL_Rect dest_rect;
    SDL_Texture *texture;
};

Entity create_entity(const char *texture_name, SDL_Renderer *renderer);

#endif
