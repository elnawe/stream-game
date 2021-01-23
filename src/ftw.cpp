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
global_var Tilemap rooms[FLOOR_SIZE][FLOOR_SIZE];

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
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
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

inline void recanonicalize_coords(World *world,
                                  i32 tile_count,
                                  s32 *tilemap_pos,
                                  s32 *tile_pos,
                                  r32 *tile_relative_pos) {
    s32 offset = floor_r32_to_s32(*tile_relative_pos / world->tile_side_meters);

    *tile_pos += offset;

    *tile_relative_pos -= offset*world->tile_side_meters;

    assert(*tile_relative_pos >= 0);
    assert(*tile_relative_pos < world->tile_side_meters);

    if (*tile_pos < 0) {
        *tile_pos = tile_count + *tile_pos;
        --*tilemap_pos;
    }

    if (*tile_pos >= tile_count) {
        *tile_pos = *tile_pos - tile_count;
        ++*tilemap_pos;
    }
};

inline World_Position recanonicalize_position(World *world, World_Position position) {
    World_Position result = position;

    recanonicalize_coords(world,
                          world->count_x,
                          &result.tilemap_x,
                          &result.tile_x,
                          &result.tile_relative_x);
    recanonicalize_coords(world,
                          world->count_y,
                          &result.tilemap_y,
                          &result.tile_y,
                          &result.tile_relative_y);

    return result;
}

Tilemap *get_tilemap(World *world, i32 tilemap_x, i32 tilemap_y) {
    Tilemap *tilemap = 0;

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
        if (test_tile_x >= 0 && test_tile_x < world->count_x * world->tile_side_pixels &&
            test_tile_y >= 0 && test_tile_y < world->count_y * world->tile_side_pixels) {
            u32 tile_value = get_tile_value(world, tilemap, test_tile_x, test_tile_y);

            is_empty  = (tile_value == 0);
        }
    }

    return is_empty;

}

bool is_world_point_empty(World *world, World_Position world_position) {
    bool is_empty = false;

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
        // Random Seed
        srand(time(NULL));

        // TODO: Legacy code to be removed once we start using
        //       the permanent/transient storage.
        // TODO: remove this with new load_texture/create_entity combo
        game = game;
        create_entity("assets/player.bmp", &game->entity_list, game->renderer);

        // Player
        game_state->player_p.tilemap_x = FLOOR_SIZE / 2;
        game_state->player_p.tilemap_y = FLOOR_SIZE / 2;
        game_state->player_p.tile_x = 3;
        game_state->player_p.tile_y = 3;
        game_state->player_p.tile_relative_x = 5.0f;
        game_state->player_p.tile_relative_y = 5.0f;


        game->entity_list.e[0].type = PLAYER;
        game->entity_list.e[0].size.h = 1.4f;
        game->entity_list.e[0].size.w = 0.60f*game->entity_list.e[0].size.h;

        memory->is_initialized = true;
    }

    Vector2 movement = {};
    Entity *player = &game->entity_list.e[0];

    current = SDL_GetTicks();

    if (input->kb.move_left.is_down) {
        movement.x = -1.0f;
        player->flip_mode = SDL_FLIP_HORIZONTAL;
    }
    if (input->kb.move_right.is_down) {
        movement.x = 1.0f;
        player->flip_mode = SDL_FLIP_NONE;
    }
    if (input->kb.move_up.is_down) {
        movement.y = -1.0f;
    }
    if (input->kb.move_down.is_down) {
        movement.y = 1.0f;
    }

    // Entity *player = entity_get_player(game);
    // TODO: this is meters per second.
    f32 velocity = 8.0f * game->delta_time;
    Vector2 d_player_position = movement * velocity;

    player->src_rect.w = 16;
    player->src_rect.h = 16;
    player->src_rect.y = 16;

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

    //game_floor.rooms = game_state->world_map;

    local_var u32 gen_pos_x = FLOOR_SIZE / 2;
    local_var u32 gen_pos_y = FLOOR_SIZE / 2;

    floor_rooms[gen_pos_y][gen_pos_x] = 1;

    while (!game_state->world_generated &&
           game_floor.discovered < game_floor.total_rooms) {
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
            game_floor.discovered++;
        }

        game_state->world_map = (u32 *)floor_rooms;
    }

    World world = {};

    world.tile_side_meters = 1.4f;
    world.tile_side_pixels = 70;
    world.meters_to_pixels = (r32)world.tile_side_pixels / (r32)world.tile_side_meters;

    world.tile_count_x = FLOOR_SIZE;
    world.tile_count_y = FLOOR_SIZE;
    world.count_x = TILEMAP_SIZE_X;
    world.count_y = TILEMAP_SIZE_Y;
    world.offset_x = TILEMAP_SIZE_X / 2;
    world.offset_y = game->window_height - 110;

    local_var bool gen_done;
    local_var u32 gen_y, gen_x;

    while (!game_state->world_generated && !gen_done) {
        if (game_state->world_map[gen_y*FLOOR_SIZE + gen_x] == 1) {
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

        rooms[gen_y][gen_x].colorR = random_range(20, 220);
        rooms[gen_y][gen_x].colorG = random_range(20, 220);
        rooms[gen_y][gen_x].colorB = random_range(20, 220);

        gen_x++;

        // TODO: This save to game_state will need to be removed once tiles
        //       are converted into entities.
        if (gen_y == FLOOR_SIZE) {
            gen_done = true;
            game_state->world_generated = true;

            for (int X = 0; X < 22; ++X) {
                for (int Y = 0; Y < 22; ++Y) {
                    game_state->rooms[Y][X] = rooms[Y][X];
                }
            }
        }

        if (gen_x == FLOOR_SIZE) {
            gen_x = 0;
            gen_y++;
        }
    }

    world.tilemaps = (Tilemap *)game_state->rooms;

    // render_tilemap(game);
    // TILEMAP
    Tilemap *tilemap = get_tilemap(&world,
                                   game_state->player_p.tilemap_x,
                                   game_state->player_p.tilemap_y);
    assert(tilemap);

    World_Position new_player_position = game_state->player_p;
    new_player_position.tile_relative_x += d_player_position.x;
    new_player_position.tile_relative_y -= d_player_position.y;
    new_player_position = recanonicalize_position(&world, new_player_position);

    World_Position new_player_left = new_player_position;
    new_player_left.tile_relative_x -= 0.5f*player->size.w;
    new_player_left = recanonicalize_position(&world, new_player_left);

    World_Position new_player_right = new_player_position;
    new_player_right.tile_relative_x += 0.5f*player->size.w;
    new_player_right = recanonicalize_position(&world, new_player_right);

    if (is_world_point_empty(&world, new_player_position) &&
        is_world_point_empty(&world, new_player_left) &&
        is_world_point_empty(&world, new_player_right)) {
        game_state->player_p = new_player_position;
    }

    for (int row = 0; row < world.count_y; ++row) {
        for (int column = 0; column < world.count_x; ++column) {
            SDL_Rect dst_rect;
            i32 tile_value = get_tile_value(&world, tilemap, column, row);

            dst_rect.x = world.offset_x + (column * world.tile_side_pixels);
            dst_rect.y = world.offset_y - (row * world.tile_side_pixels);
            dst_rect.w = world.tile_side_pixels;
            dst_rect.h = world.tile_side_pixels;

            // TODO: This is only demo testing
            SDL_SetRenderDrawColor(game->renderer,
                                   tilemap->colorR,
                                   tilemap->colorG,
                                   tilemap->colorB, 255);

            if (tile_value == 1) {
                SDL_SetRenderDrawColor(game->renderer, 50, 50, 50, 255);
            }

            if (column == game_state->player_p.tile_x &&
                row == game_state->player_p.tile_y) {
                SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
            }

            SDL_RenderFillRect(game->renderer, &dst_rect);
        }
    }

    SDL_FRect player_rect;

    player_rect.x = world.offset_x +
        world.tile_side_pixels*game_state->player_p.tile_x +
        world.meters_to_pixels*game_state->player_p.tile_relative_x -
        0.5f*(world.meters_to_pixels*player->size.w);
    // TODO: Adding world.tile_side_pixels because it's missing
    // from the original math. Check later.
    player_rect.y = world.tile_side_pixels + world.offset_y -
        world.tile_side_pixels*game_state->player_p.tile_y -
        world.meters_to_pixels*game_state->player_p.tile_relative_y -
        world.meters_to_pixels*player->size.h;
    player_rect.w = world.meters_to_pixels*player->size.w;
    player_rect.h = world.meters_to_pixels*player->size.h;

    SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
    SDL_RenderFillRectF(game->renderer, &player_rect);

    for (i32 i = 0; i < game->entity_list.count; ++i) {
        continue;
        Entity e = game->entity_list.e[i];

        if (e.type != NONE && e.type != ITEM) {
            SDL_FRect dst_rect;

            dst_rect.x = e.position.x;
            dst_rect.y = e.position.y;
            dst_rect.w = e.size.w;
            dst_rect.h = e.size.h;

            if (e.type == PLAYER) {
                dst_rect.x = world.offset_x +
                    world.tile_side_pixels*game_state->player_p.tile_x +
                    world.meters_to_pixels*game_state->player_p.tile_relative_x -
                    0.5f*(world.meters_to_pixels*e.size.w);
                dst_rect.y = world.offset_y -
                    world.tile_side_pixels*game_state->player_p.tile_y -
                    world.meters_to_pixels*game_state->player_p.tile_relative_y -
                    world.meters_to_pixels*e.size.h;
                dst_rect.w = world.meters_to_pixels*e.size.w;
                dst_rect.h = world.meters_to_pixels*e.size.h;
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

            SDL_RenderCopyExF(game->renderer,
                             e.texture,
                             &e.src_rect,
                             &dst_rect,
                             0, NULL, e.flip_mode);
        }
    }
}
