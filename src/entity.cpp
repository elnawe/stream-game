#include <stdio.h>
#include "SDL.h"
#include "entity.h"
#include "math.h"

Entity create_entity(const char *texture_name, SDL_Renderer *renderer) {
    Entity new_entity;
    SDL_Surface *temp_surface = SDL_LoadBMP(texture_name);
    SDL_SetColorKey(temp_surface, SDL_TRUE, SDL_MapRGB(temp_surface->format, 118, 66, 138));
    SDL_Rect src_rect, dest_rect;

    if (temp_surface == NULL) {
        fprintf(stderr, "[SDL] Error loading surface: %s\n", SDL_GetError());
    }

    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.w = temp_surface->w;
    src_rect.h = temp_surface->h;

    dest_rect.x = 0;
    dest_rect.y = 0;
    dest_rect.w = temp_surface->w;
    dest_rect.h = temp_surface->h;

    new_entity.position = Vector2{0, 0};
    new_entity.texture = SDL_CreateTextureFromSurface(renderer, temp_surface);
    new_entity.src_rect = src_rect;
    new_entity.dest_rect = dest_rect;

    SDL_FreeSurface(temp_surface);

    return new_entity;
}
