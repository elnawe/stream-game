#include "entity.h"

#include <stdio.h>
#include <string.h>

#include "ftw.h"
#include "item.h"
#include "math.h"
#include "SDL.h"
#include "texture_manager.h"

Entity *create_entity(const char *surface_name,
                      Entity_List *entity_list,
                      SDL_Renderer *renderer) {
    int current = entity_list->count;
    Entity new_entity;
    Vector2 position, size;
    SDL_Rect src_rect;

    SDL_Surface *temp_surface = SDL_LoadBMP(surface_name);

    if (temp_surface == NULL) {
        fprintf(stderr, "[SDL] Error loading surface: %s\n", SDL_GetError());
    }

    SDL_SetColorKey(temp_surface, SDL_TRUE, SDL_MapRGB(temp_surface->format, 118, 66, 138));

    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.w = temp_surface->w;
    src_rect.h = temp_surface->h;

    position.x = 0;
    position.y = 0;
    size.x = 16;
    size.y = 16;

    new_entity.id = current;
    new_entity.position = position;
    new_entity.size = size;
    new_entity.texture = SDL_CreateTextureFromSurface(renderer, temp_surface);
    new_entity.flip_mode = SDL_FLIP_NONE;
    new_entity.src_rect = src_rect;

    SDL_FreeSurface(temp_surface);

    entity_list->e[current] = new_entity;
    entity_list->count++;

    return &entity_list->e[current];
}

int create_entity_from_item(Item item,
                            Entity_List *entity_list,
                            Texture_Manager *texture_manager) {
    SDL_Texture *texture;
    int current = entity_list->count;
    Entity new_entity;

    for (int i = 0; i < texture_manager->count; ++i) {
        if (strcmp(texture_manager->tl[i].name, item.texture_name) == 0) {
            texture = texture_manager->tl[i].texture;
            break;
        }
    }

    if (texture == NULL) {
        fprintf(stderr, "[Entity] Error loading texture");
    }

    new_entity.id = current;
    new_entity.size = item.size;

    new_entity.type = ITEM;

    new_entity.flip_mode = SDL_FLIP_NONE;
    new_entity.src_rect.x = item.frame_position.x;
    new_entity.src_rect.y = item.frame_position.y;
    new_entity.src_rect.w = item.frame_size.x;
    new_entity.src_rect.h = item.frame_size.y;
    new_entity.texture = texture;

    entity_list->e[current] = new_entity;
    entity_list->count++;

    return new_entity.id;
};
