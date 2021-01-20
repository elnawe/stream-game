#include "ftw_config.h"
#include "ftw_math.h"
#include "ftw_intrinsics.h"
#include "ftw_types.h"

/* TODO: (read below)
 *     All these below might need clean up or shouldn't be here.
 *     Make sure all internal dependency starts with ftw_ this point forward,
 *     also, make the platform layer debug the code, not the game itself.
 */
#include <stdio.h>
#include <stdlib.h>
#include "entity.h"
#include "item.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#include "texture_manager.h"

// Compile only these...
#include "entity.cpp"
#include "item.cpp"
#include "texture_manager.cpp"

// TEMP VARS
static Game_Data *game;
static Uint64 animation_timer;
static Uint32 current;
static Uint32 last_update;
static bool show_box_colliders = true;
static Uint32 fullscreen_toggle = 0;

const u32 TILEMAP_SIZE_X = 17;
const u32 TILEMAP_SIZE_Y = 9;

global_var u32 reset_map_gen[TILEMAP_SIZE_Y][TILEMAP_SIZE_X] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

global_var u32 map_gen[TILEMAP_SIZE_Y][TILEMAP_SIZE_X] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

struct Tilemap {
    u32 *tiles;
};

struct World {
    u32 tile_count_x;
    u32 tile_count_y;


    u32 count_x;
    u32 count_y;
    i32 offset_x;
    i32 offset_y;
    i32 width;
    i32 height;

    Tilemap *tilemaps;
};

struct World_Position {
    i32 tilemap_x;
    i32 tilemap_y;

    i32 tile_x;
    i32 tile_y;

    // NOTE: X and Y relative to the tile
    //       (top-left corner of the tile)
    r32 tile_relative_x;
    r32 tile_relative_y;
};

struct Raw_Position {
    i32 tilemap_x;
    i32 tilemap_y;

    // NOTE: X and Y relative to the tilemap
    //       (top-left corner of the screen)
    r32 x;
    r32 y;
};

inline World_Position get_world_position(World *world, Raw_Position position) {
    World_Position result;

    result.tilemap_x = position.tilemap_x;
    result.tilemap_y = position.tilemap_y;

    r32 X = position.x - world->offset_x;
    r32 Y = position.y - world->offset_y;
    result.tile_x = floor_r32_to_i32(X / world->width);
    result.tile_y = floor_r32_to_i32(Y / world->height);

    result.tile_relative_x = X - (result.tile_x*world->width);
    result.tile_relative_y = Y - (result.tile_y*world->height);

    assert(result.tile_relative_x >= 0);
    assert(result.tile_relative_y >= 0);
    assert(result.tile_relative_x < world->width);
    assert(result.tile_relative_y < world->height);

    if (result.tile_x < 0) {
        result.tile_x = world->count_x + result.tile_x;
        --result.tilemap_x;
    }

    if (result.tile_y < 0) {
        result.tile_y = world->count_y + result.tile_y;
        --result.tilemap_y;
    }

    if (result.tile_x >= world->count_x) {
        result.tile_x = result.tile_x - world->count_x;
        ++result.tilemap_x;
    }

    if (result.tile_y >= world->count_y) {
        result.tile_y = result.tile_y - world->count_y;
        ++result.tilemap_y;
    }

    return result;
}

Tilemap *get_tilemap(World *world, i32 tilemap_x, i32 tilemap_y) {
    Tilemap *tilemap = 0;

    if (tilemap_x >= 0 && tilemap_x < world->tile_count_x &&
        tilemap_y >= 0 && tilemap_y < world->tile_count_y) {
        tilemap = &world->tilemaps[tilemap_y * world->tile_count_x + tilemap_x];
    }

    return tilemap;
}

u32 get_tile_value(World *world, Tilemap *tilemap, i32 tile_x, i32 tile_y) {
    assert(tilemap);
    assert((tile_x >= 0) && (tile_x < world->count_x) &&
           (tile_y >= 0) && (tile_y < world->count_y));

    u32 tile_value = tilemap->tiles[tile_y * world->count_x + tile_x];

    return tile_value;
}


bool is_tilemap_point_empty(World *world,
                            Tilemap *tilemap,
                            i32 test_tile_x,
                            i32 test_tile_y) {
    bool is_empty = false;

    if (tilemap) {
        if (test_tile_x >= 0 && test_tile_x < world->count_x * world->width &&
            test_tile_y >= 0 && test_tile_y < world->count_y * world->height) {
            u32 tile_value = get_tile_value(world, tilemap, test_tile_x, test_tile_y);

            is_empty  = (tile_value == 0);
        }
    }

    return is_empty;

}

bool is_world_point_empty(World *world, Raw_Position test_position) {
    bool is_empty = false;

    World_Position world_position = get_world_position(world, test_position);

    Tilemap *tilemap = get_tilemap(world,
                                   world_position.tilemap_x,
                                   world_position.tilemap_y);

    is_empty = is_tilemap_point_empty(world,
                                      tilemap,
                                      world_position.tile_x,
                                      world_position.tile_y);

    return is_empty;
}

extern "C" GAME_UPDATE_AND_RENDER(game_update_and_render) {
    assert(sizeof(Game_State) <= memory->permanent_storage_size);

    Game_State *game_state = (Game_State *)memory->permanent_storage;

    if (!memory->is_initialized) {
        game = game;
        // TODO: Legacy code to be removed once we start using
        //       the permanent/transient storage.
        // TODO: remove this with new load_texture/create_entity combo
        create_entity("assets/player.bmp", &game->entity_list, game->renderer);

        // MUSIC
        Mix_Music *bgm = Mix_LoadMUS("assets/bgm.mp3");
        Mix_VolumeMusic(game->options.music_volume * MIX_MAX_VOLUME);
        Mix_PlayMusic(bgm, -1);

        // NOTE Game stats here
        // TEXTURES
        load_texture("ITEMS", "items.bmp", &game->texture_manager, game->renderer);
        load_texture("TILESET", "tileset.png", &game->texture_manager, game->renderer);

        // ITEMS
        create_item("pepsi", "ITEMS", Vector2{0, 0}, Vector2{20, 32}, &game->item_list);
        create_item("mayo", "ITEMS", Vector2{32, 0}, Vector2{24, 32}, &game->item_list);
        create_item("hamburger", "ITEMS", Vector2{64, 0}, Vector2{28, 30}, &game->item_list);
        create_item("fries", "ITEMS", Vector2{96, 0}, Vector2{24, 30}, &game->item_list);
        create_item("popsicle", "ITEMS", Vector2{128, 0}, Vector2{15, 30}, &game->item_list);
        create_item("pineapple_pizza", "ITEMS", Vector2{160, 0}, Vector2{32, 30}, &game->item_list);
        create_item("whiskey", "ITEMS", Vector2{192, 0}, Vector2{24, 28}, &game->item_list);
        create_item("beer", "ITEMS", Vector2{224, 0}, Vector2{28, 32}, &game->item_list);
        create_item("muffin", "ITEMS", Vector2{256, 0}, Vector2{26, 24}, &game->item_list);
        create_item("hotdog", "ITEMS", Vector2{288, 0}, Vector2{32, 18}, &game->item_list);
        create_item("steak", "ITEMS", Vector2{0, 32}, Vector2{26, 32}, &game->item_list);

        // ENTITIES
        for (int i = 0; i < game->item_list.count; ++i) {
            int entity_id = create_entity_from_item(game->item_list.il[i],
                                                    &game->entity_list,
                                                    &game->texture_manager);
            game->entity_list.e[entity_id].position.x =
                ((game->screen_width / game->item_list.count) * i) + 16;
            game->entity_list.e[entity_id].position.y =
                (game->screen_height / 2) + 32;
        }

        // Player
        game_state->player_position = { 200, 200 };

        game->entity_list.e[0].type = PLAYER;
        game->entity_list.e[0].size.w = 24;
        game->entity_list.e[0].size.h = 32;
        game->entity_list.e[0].position = game_state->player_position;

        memory->is_initialized = true;
    }

    Vector2 movement = {};
    Entity *player = &game->entity_list.e[0];

    current = SDL_GetTicks();

    if (input->kb.move_left.is_down) {
        movement.x = -1;
        player->flip_mode = SDL_FLIP_HORIZONTAL;
    }
    if (input->kb.move_right.is_down) {
        movement.x = 1;
        player->flip_mode = SDL_FLIP_NONE;
    }
    if (input->kb.move_up.is_down) {
        movement.y = -1;
    }
    if (input->kb.move_down.is_down) {
        movement.y = 1;
    }

    // Entity *player = entity_get_player(game);
    // TODO Move to player entity
    f32 velocity = 128.0f * game->delta_time;
    Vector2 d_player_position = movement * velocity;

    player->src_rect.w = 16;
    player->src_rect.h = 16;
    player->src_rect.y = 16;
    player->position = game_state->player_position;

    // TODO: FIX MEEEEEE!!!111
    r32 animation_time = (current - last_update) / 1000.0f;
    r64 frame_to_update = floor(animation_time / (1.0f / 6.0f));

    if (frame_to_update > 0) {
        last_update = current;
        player->src_rect.x += 16;

        if (player->src_rect.x > 16) {
            player->src_rect.x = 0;
        }
    }

    // render_tilemap(game);
    // TILEMAP
    World world = {};

    world.tile_count_x = 3;
    world.tile_count_y = 2;
    world.count_x = TILEMAP_SIZE_X;
    world.count_y = TILEMAP_SIZE_Y;
    world.offset_x = -5;
    world.offset_y = -5;
    world.width = 42;
    world.height = 40;

    u32 tiles00[TILEMAP_SIZE_Y][TILEMAP_SIZE_X] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1}
    };

    u32 tiles01[TILEMAP_SIZE_Y][TILEMAP_SIZE_X] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1}
    };

    u32 tiles02[TILEMAP_SIZE_Y][TILEMAP_SIZE_X] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };

    u32 tiles10[TILEMAP_SIZE_Y][TILEMAP_SIZE_X] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
        {1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };

    u32 tiles11[TILEMAP_SIZE_Y][TILEMAP_SIZE_X] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };

    Tilemap tilemaps[2][3];

    tilemaps[0][0].tiles = (u32 *)tiles00;
    tilemaps[0][1].tiles = (u32 *)tiles01;
    tilemaps[0][2].tiles = (u32 *)tiles02;
    tilemaps[1][0].tiles = (u32 *)tiles10;
    tilemaps[1][1].tiles = (u32 *)tiles11;

    world.tilemaps = (Tilemap *)tilemaps;

    Vector2 new_player_position = game_state->player_position + d_player_position;

    Tilemap *tilemap = get_tilemap(&world,
                                   game_state->player_tilemap_x,
                                   game_state->player_tilemap_y);
    assert(tilemap);

    Raw_Position player_position = {
        game_state->player_tilemap_x,
        game_state->player_tilemap_y,
        new_player_position.x,
        new_player_position.y
    };
    Raw_Position player_left = player_position;
    player_left.x -= 0.5f*player->size.w;
    Raw_Position player_right = player_position;
    player_right.x += 0.5f*player->size.w;

    if (is_world_point_empty(&world, player_position) &&
        is_world_point_empty(&world, player_left) &&
        is_world_point_empty(&world, player_right)) {
        World_Position new_position = get_world_position(&world, player_position);

        game_state->player_tilemap_x = new_position.tilemap_x;
        game_state->player_tilemap_y = new_position.tilemap_y;
        game_state->player_position.x =
            world.offset_x + world.width * new_position.tile_x + new_position.tile_relative_x;
        game_state->player_position.y =
            world.offset_y + world.height * new_position.tile_y + new_position.tile_relative_y;

    }

    for (int row = 0; row < world.count_y; ++row) {
        for (int column = 0; column < world.count_x; ++column) {
            SDL_Rect dst_rect;
            i32 tile_value = get_tile_value(&world, tilemap, column, row);

            dst_rect.x = world.offset_x + (column * world.width);
            dst_rect.y = world.offset_y + (row * world.height);
            dst_rect.w = world.width;
            dst_rect.h = world.height;

            SDL_SetRenderDrawColor(game->renderer, 190, 190, 190, 255);

            if (tile_value == 1) {
                SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
            }

            SDL_RenderFillRect(game->renderer, &dst_rect);
        }
    }

    for (i32 i = 0; i < game->entity_list.count; ++i) {
        Entity e = game->entity_list.e[i];

        if (e.type != NONE && e.type != ITEM) {
            SDL_Rect dst_rect;

            dst_rect.x = e.position.x;
            dst_rect.y = e.position.y;
            dst_rect.w = e.size.w;
            dst_rect.h = e.size.h;

            if (e.type == PLAYER) {
                dst_rect.x = game_state->player_position.x - 0.5f*e.size.w;
                dst_rect.y = game_state->player_position.y - e.size.h;
            }

#if FTW_INTERNAL
            // Render the box collider
            if (show_box_colliders) {
                SDL_Rect box_collider;

                box_collider.x = dst_rect.x - 1;
                box_collider.y = dst_rect.y - 1;
                box_collider.w = dst_rect.w + 2;
                box_collider.h = dst_rect.h + 2;


                SDL_SetRenderDrawColor(game->renderer, 0, 255, 0, 255);
                SDL_RenderDrawRect(game->renderer, &box_collider);
            }
#endif

            SDL_RenderCopyEx(game->renderer,
                             e.texture,
                             &e.src_rect,
                             &dst_rect,
                             0, NULL, e.flip_mode);
        }
    }

#if 0
    /* TODO:/NOTE:
     *   This code is part of the world generator and it will conflict
     *   with the tilemap above. Move to its own file once it's done.
     */
    world.offset_x = 0;
    world.offset_y = 0;

    u32 map_center_x = TILEMAP_SIZE_X / 2;
    u32 map_center_y = TILEMAP_SIZE_Y / 2;
    local_var u32 gen_pos_x = map_center_x;
    local_var u32 gen_pos_y = map_center_y;
    local_var u32 step = 1;
    local_var u32 last_step = 0;
    local_var u32 discovered = 1;

    enum MOVE_DIR {
        MOVE_UP,
        MOVE_RIGHT,
        MOVE_DOWN,
        MOVE_LEFT
    };

    Tilemap map_gen_tilemap;

    map_gen[map_center_y][map_center_x] = 2;

    map_gen_tilemap.tiles = (u32 *)map_gen;

    u32 MAX_ROOMS = 15;

    if (input->kb.action.is_down && discovered < MAX_ROOMS) {
        if (step != last_step) {
            i32 X, Y;
            u32 new_direction = rand() % 4;

            if (new_direction == MOVE_UP) {
                gen_pos_y -= 1;
            } else if (new_direction == MOVE_RIGHT) {
                gen_pos_x += 1;
            } else if (new_direction == MOVE_DOWN) {
                gen_pos_y += 1;
            } else if (new_direction == MOVE_LEFT) {
                gen_pos_x -= 1;
            }

            if (map_gen[gen_pos_y][gen_pos_x] == 0) {
                map_gen[gen_pos_y][gen_pos_x] = 1;
                discovered++;
            }

            last_step = step;
        } else {
            step++;
        }
    } else if (input->kb.reset.is_down && discovered == MAX_ROOMS) {
        for (int Y = 0; Y < world.count_y; ++Y) {
            for (int X = 0; X < world.count_x; ++X) {
                map_gen_tilemap.tiles[Y * world.count_x + X] = 0;
            }
        }

        gen_pos_x = map_center_x;
        gen_pos_y = map_center_y;
        discovered = 1;
    }

    for (int Y = 0; Y < world.count_y; ++Y) {
        for (int X = 0; X < world.count_x; ++X) {
            SDL_Rect dst_rect;
            u32 tile_value = get_tile_value(&world, &map_gen_tilemap, X, Y);

            dst_rect.x = world.offset_x + (X * world.width);
            dst_rect.y = world.offset_y + (Y * world.height);
            dst_rect.w = world.width;
            dst_rect.h = world.height;

            SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);

            if (tile_value == 1) {
                SDL_SetRenderDrawColor(game->renderer, 0, 0, 255, 255);
            }

            if (tile_value == 2) {
                SDL_SetRenderDrawColor(game->renderer, 0, 255, 0, 255);
            }

            SDL_RenderFillRect(game->renderer, &dst_rect);
        }
    }

    SDL_Rect track_rect;
    track_rect.x = gen_pos_x*world.width;
    track_rect.y = gen_pos_y*world.height;
    track_rect.w = world.width;
    track_rect.h = world.height;

    SDL_SetRenderDrawColor(game->renderer, 255, 255, 0, 255);
    SDL_RenderFillRect(game->renderer, &track_rect);

#endif
}
