#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "SDL.h"

struct Texture {
    const char *name;
    SDL_Texture *texture;
};

struct Texture_Manager {
    Texture tl[5];
    int count;
};

void load_texture(int texture_id,
                  const char *surface_name,
                  Texture_Manager *texture_manager,
                  SDL_Renderer *renderer);

#endif
