#ifndef ENTITY_H
#define ENTITY_H

#include "item.h"
#include "math.h"
#include "SDL.h"
#include "texture_manager.h"

enum Entity_Type {
                  NONE,
                  PLAYER,
                  ENEMY,
                  ITEM
};

struct Entity {
    int id;

    Vector2 position;
    Vector2 size;

    Entity_Type type;

    SDL_RendererFlip flip_mode;
    SDL_Rect src_rect;
    SDL_Texture *texture;
};

struct Entity_List {
    Entity e[10000];
    int count;
};

Entity *create_entity(const char *surface_name,
                      Entity_List *entity_list,
                      SDL_Renderer *renderer);

int create_entity_from_item(Item item,
                            Entity_List *entity_list,
                            Texture_Manager *texture_manager);

#endif
