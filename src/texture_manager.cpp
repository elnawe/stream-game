#include "texture_manager.h"

#include <string.h>
#include "SDL.h"
#include "SDL_image.h"

void load_texture(const char *name,
                  const char *surface_name,
                  Texture_Manager *texture_manager,
                  SDL_Renderer *renderer) {
    char surface_filename[50];

    strcpy(surface_filename, "assets/");
    strcat(surface_filename, surface_name);

    SDL_Surface *temp_surface = IMG_Load(surface_filename);
    Texture new_texture;

    if (temp_surface == NULL) {
        fprintf(stderr, "[Texture_Manager] Error loading surface: %s\n", IMG_GetError());
    }

    SDL_SetColorKey(temp_surface, SDL_TRUE, SDL_MapRGB(temp_surface->format, 118, 66, 138));

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, temp_surface);

    new_texture.name = name;
    new_texture.texture = texture;
    texture_manager->tl[texture_manager->count] = new_texture;
    texture_manager->count++;

    SDL_FreeSurface(temp_surface);
}

SDL_Texture *get_texture(const char *name, Texture_Manager *texture_manager) {
    SDL_Texture *texture;

    for (int i = 0; i < texture_manager->count; ++i) {
        if (strcmp(texture_manager->tl[i].name, name) == 0) {
            texture = texture_manager->tl[i].texture;
            break;
        }
    }

    return texture;
}
