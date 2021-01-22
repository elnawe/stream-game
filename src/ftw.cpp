#include "ftw_config.h"
#include "ftw_math.h"
#include "ftw_intrinsics.h"
#include "ftw_types.h"
#include "ftw_world.h"

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
const u32 FLOOR_SIZE = 22;
const u32 TILEMAP_SIZE_X = 17;
const u32 TILEMAP_SIZE_Y = 9;

global_var Game_Data *game;
global_var Uint64 animation_timer;
global_var Uint32 current;
global_var Uint32 last_update;
global_var bool show_box_colliders = true;
global_var Uint32 fullscreen_toggle = 0;
global_var Floor game_floor;
global_var World world = {};
global_var Tilemap rooms[FLOOR_SIZE][FLOOR_SIZE];
global_var u8 try_colorR = random_range(20, 255, 1278903718);
global_var u8 try_colorG = random_range(20, 255, 9456748956);
global_var u8 try_colorB = random_range(20, 255, 45873894579);

global_var u32 floor_rooms[FLOOR_SIZE][FLOOR_SIZE] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

global_var u32 room_nesw00[TILEMAP_SIZE_Y][TILEMAP_SIZE_X] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1}
};

global_var u32 room_nesw01[TILEMAP_SIZE_Y][TILEMAP_SIZE_X] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1}
};

global_var u32 room_nesw02[TILEMAP_SIZE_Y][TILEMAP_SIZE_X] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1}
};

global_var u32 room_nesw03[TILEMAP_SIZE_Y][TILEMAP_SIZE_X] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1}
};

global_var u32 room_no_exit[TILEMAP_SIZE_Y][TILEMAP_SIZE_X] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
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
    Tilemap *tilemap;// = 0;

    if (tilemap_x >= 0 && tilemap_x < world->tile_count_x &&
        tilemap_y >= 0 && tilemap_y < world->tile_count_y) {
        tilemap = &world->tilemaps[tilemap_y * world->tile_count_x + tilemap_x];
    }

    assert(tilemap);

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
        game_state->player_tilemap_x = FLOOR_SIZE / 2;
        game_state->player_tilemap_y = FLOOR_SIZE / 2;

        game->entity_list.e[0].type = PLAYER;
        game->entity_list.e[0].size.w = 24;
        game->entity_list.e[0].size.h = 32;
        game->entity_list.e[0].position = game_state->player_position;

        ///
        /// FLOOR STATS HERE
        /// TODO: Check this out! Floor logic
        ///
        game_floor.count_x = FLOOR_SIZE;
        game_floor.count_y = FLOOR_SIZE;
        game_floor.max_rooms = 20;
        game_floor.min_rooms = 8;
        game_floor.total_rooms =
            game_floor.min_rooms +
            random(game_floor.max_rooms - game_floor.min_rooms);

        game_floor.rooms = (u32 *)floor_rooms;

        game_state->floor = game_floor;

        local_var u32 gen_pos_x = FLOOR_SIZE / 2;
        local_var u32 gen_pos_y = FLOOR_SIZE / 2;

        floor_rooms[gen_pos_y][gen_pos_x] = 1;

        while (game_state->floor.discovered < game_state->floor.total_rooms) {
            u32 new_direction = random(3);

            if (new_direction == FloorGenMove_Up && gen_pos_y > 0) {
                gen_pos_y -= 1;
            } else if (new_direction == FloorGenMove_Right && gen_pos_x < FLOOR_SIZE) {
                gen_pos_x += 1;
            } else if (new_direction == FloorGenMove_Down && gen_pos_y < FLOOR_SIZE) {
                gen_pos_y += 1;
            } else if (new_direction == FloorGenMove_Left && gen_pos_x > 0) {
                gen_pos_x -= 1;
            }

            if (floor_rooms[gen_pos_y][gen_pos_x] == 0) {
                floor_rooms[gen_pos_y][gen_pos_x] = 1;
                game_state->floor.discovered++;
            }
        }

        world.tile_count_x = FLOOR_SIZE;
        world.tile_count_y = FLOOR_SIZE;
        world.count_x = TILEMAP_SIZE_X;
        world.count_y = TILEMAP_SIZE_Y;
        world.offset_x = -5;
        world.offset_y = -5;
        world.width = 42;
        world.height = 40;

        for (u32 gen_y = 0; gen_y < FLOOR_SIZE; ++gen_y) {
            for (u32 gen_x = 0; gen_x < FLOOR_SIZE; ++gen_x) {
                if (game_state->floor.rooms[gen_y*FLOOR_SIZE + gen_x] == 1) {
                    u32 pick_room = random(3);

                    switch(pick_room) {
                        case 0: {
                            rooms[gen_y][gen_x].tiles = (u32 *)room_nesw00;
                        } break;
                        case 1: {
                            rooms[gen_y][gen_x].tiles = (u32 *)room_nesw01;
                        } break;
                        case 2: {
                            rooms[gen_y][gen_x].tiles = (u32 *)room_nesw02;
                        } break;
                        case 3: {
                            rooms[gen_y][gen_x].tiles = (u32 *)room_nesw03;
                        } break;
                    }
                } else {
                    rooms[gen_y][gen_x].tiles = (u32 *)room_no_exit;
                }
            }
        }

        world.tilemaps = (Tilemap *)rooms;
        game_state->world = world;

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

    Vector2 new_player_position = game_state->player_position + d_player_position;

    Tilemap *tilemap = get_tilemap(&game_state->world,
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

    if (is_world_point_empty(&game_state->world, player_position) &&
        is_world_point_empty(&game_state->world, player_left) &&
        is_world_point_empty(&game_state->world, player_right)) {
        World_Position new_position = get_world_position(&game_state->world, player_position);

        if ((game_state->player_tilemap_x != new_position.tilemap_x) ||
            (game_state->player_tilemap_y != new_position.tilemap_y)) {
            printf("NEW ROOM DISCOVERED: (%d, %d)\n", new_position.tilemap_x, new_position.tilemap_y);
            try_colorR = random_range(20, 255, SDL_GetTicks() * 345);
            try_colorG = random_range(20, 255, SDL_GetTicks() * 890);
            try_colorB = random_range(20, 255, SDL_GetTicks() * 39124);
        }

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
            i32 tile_value = get_tile_value(&game_state->world, tilemap, column, row);

            dst_rect.x = world.offset_x + (column * world.width);
            dst_rect.y = world.offset_y + (row * world.height);
            dst_rect.w = world.width;
            dst_rect.h = world.height;

            SDL_SetRenderDrawColor(game->renderer, try_colorR, try_colorG, try_colorB, 255);

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
}
