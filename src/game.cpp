#include "game.h"

#include <stdio.h>

#include "config.h"
#include "entity.h"
#include "item.h"
#include "math.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#include "texture_manager.h"
#include "types.h"
//#include "world.h"

// Compile only these...
#include "entity.cpp"
#include "item.cpp"
#include "texture_manager.cpp"
//#include "world.cpp"

static Game_Data *game;
static Game_State *game_state;

// TEMP VARS
static Uint64 animation_timer;
static Uint32 current;
static Uint32 last_update;
static bool show_box_colliders = true;
static Uint32 fullscreen_toggle = 0;

const u32 TILEMAP_SIZE_X = 17;
const u32 TILEMAP_SIZE_Y = 9;

struct Tilemap {
    u32 count_x;
    u32 count_y;

    i32 upper_left_x;
    i32 upper_left_y;
    u32 width;
    u32 height;

    u32 *tiles;
};

bool is_point_empty(Tilemap *tilemap, Vector2 test_point) {
    bool is_empty = false;

    u32 tile_x = (u32)((test_point.x - tilemap->upper_left_x) / tilemap->width);
    u32 tile_y = (u32)((test_point.y - tilemap->upper_left_y) / tilemap->height);

    if (test_point.x >= 0 && test_point.x < tilemap->count_x * tilemap->width &&
        test_point.y >= 0 && test_point.y < tilemap->count_y * tilemap->height) {
        is_empty  = (tilemap->tiles[tile_y * tilemap->count_x + tile_x] == 0);
    }

    return is_empty;
}

void GAME_INIT(Game_Data *g, Game_State *state) {
    game = g;
    game_state = state;
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
    game_state->player_position = {
        (game->screen_width / 2) - 16,
        (game->screen_height / 2) - 16
    };

    game->entity_list.e[0].type = PLAYER;
    game->entity_list.e[0].size.w = 24;
    game->entity_list.e[0].size.h = 32;
    game->entity_list.e[0].position = game_state->player_position;

}

void GAME_REFRESH(Game_Data *g, Game_State *state) {
    game = g;
    game_state = state;
}

void GAME_HANDLE_INPUT() {
    game_state->input = SDL_GetKeyboardState(NULL);

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT: {
                game->running = false;
            }
            case SDL_WINDOWEVENT: {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    float new_width = event.window.data1;
                    float new_height = event.window.data2;
                    float aspect_ratio_result;

                    // Keep aspect ratio defined in
                    // ASPECT_RATIO_WIDTH & ASPECT_RATIO_HEIGHT
                    if (new_width != game->window_width) {
                        aspect_ratio_result = new_width / ASPECT_RATIO_WIDTH;
                        new_height = aspect_ratio_result * ASPECT_RATIO_HEIGHT;
                    } else if (new_height != game->window_height) {
                        aspect_ratio_result = new_height / ASPECT_RATIO_HEIGHT;
                        new_width = aspect_ratio_result * ASPECT_RATIO_WIDTH;
                    }

                    // Resizing window and game scale
                    SDL_SetWindowSize(game->window, new_width, new_height);

                    SDL_RenderSetScale(game->renderer,
                                       (new_width  / ORIGINAL_GRAPHIC_WIDTH),
                                       (new_height / ORIGINAL_GRAPHIC_HEIGHT));

                    game->window_width = new_width;
                    game->window_height = new_height;
                }
            }
            case SDL_KEYDOWN: {
#if DEBUG_MODE
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    game->running = false;
                }
#endif

                if (event.key.keysym.sym == SDLK_F1) {
                    show_box_colliders = !show_box_colliders;
                }

                if (event.key.keysym.sym == SDLK_F2) {
                    if (Mix_PlayingMusic() == 0) {
                        Mix_ResumeMusic();
                    } else {
                        Mix_PauseMusic();
                    }
                }

                if (event.key.keysym.sym == SDLK_F11) {
                    if (!fullscreen_toggle) {
                        fullscreen_toggle = SDL_WINDOW_FULLSCREEN_DESKTOP;
                    } else {
                        fullscreen_toggle = 0;
                    }

                    SDL_SetWindowFullscreen(game->window, fullscreen_toggle);
                }
            }
        }
    }

    // TODO: Remove
    // if (game->entity_list.e[0].position.x < (game->screen_width/2)+32 &&
    //     game->entity_list.e[0].position.x > (game->screen_width/2)-32 &&
    //     game->entity_list.e[0].position.y < (game->screen_height/2)+32 &&
    //     game->entity_list.e[0].position.y > (game->screen_height/2)-32 &&
    //     game->entity_list.e[6].type == ITEM) {
    //     game->entity_list.e[6].type = NONE;
    //     game->entity_list.e[0].size.x *= 2;
    //     game->entity_list.e[0].size.y *= 2;
    // }
}

void GAME_UPDATE_AND_RENDER() {
    Vector2 movement;
    Entity *player = &game->entity_list.e[0];

    current = SDL_GetTicks();

    movement = {
        ((-1.0f * game_state->input[SDL_SCANCODE_A]) +
         game_state->input[SDL_SCANCODE_D]),
        ((-1.0f * game_state->input[SDL_SCANCODE_W]) +
         game_state->input[SDL_SCANCODE_S])
    };

    // Entity *player = entity_get_player(game);
    // TODO Move to player entity
    f32 velocity = 128.0f * game->delta_time;
    Vector2 d_player_position = movement * velocity;

    if (movement.x < 0) {
        player->flip_mode = SDL_FLIP_HORIZONTAL;
    } else if (movement.x > 0) {
        player->flip_mode = SDL_FLIP_NONE;
    }

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
    Tilemap tilemap = {};

    tilemap.count_x = TILEMAP_SIZE_X;
    tilemap.count_y = TILEMAP_SIZE_Y;
    tilemap.upper_left_x = -5;
    tilemap.upper_left_y = -5;
    tilemap.width = 42;
    tilemap.height = 40;

    // (COUNT_X * Y) + (X - 1)
    // (17 * 4) + (8 - 1)
    // 68 + 7
    // 75

    u32 tiles[TILEMAP_SIZE_Y][TILEMAP_SIZE_X] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
        {1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1}
    };

    u32 tiles2[TILEMAP_SIZE_Y][TILEMAP_SIZE_X] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };


    tilemap.tiles = (u32 *)tiles2;

    Vector2 new_player_position = game_state->player_position + d_player_position;

    Vector2 player_left = {
        new_player_position.x,
        new_player_position.y + (0.8f * player->size.h)
    };

    Vector2 player_right = {
        new_player_position.x + player->size.w,
        new_player_position.y + (0.8f * player->size.h)
    };

    Vector2 player_center = {
        new_player_position.x + (0.5f * player->size.w),
        new_player_position.y + (0.8f * player->size.h)
    };

    if (is_point_empty(&tilemap, player_center) &&
        is_point_empty(&tilemap, player_left) &&
       is_point_empty(&tilemap, player_right)) {
        game_state->player_position = new_player_position;
    }

    for (u32 column = 0; column < tilemap.count_x; ++column) {
        for (u32 row = 0; row < tilemap.count_y; ++row) {
            SDL_Rect dst_rect;
            u32 tile_id = tilemap.tiles[row * tilemap.count_x + column];

            dst_rect.x = tilemap.upper_left_x + (column * tilemap.width);
            dst_rect.y = tilemap.upper_left_y + (row * tilemap.height);
            dst_rect.w = tilemap.width;
            dst_rect.h = tilemap.height;

            SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);

            if (tile_id == 1) {
                SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
            }

            SDL_RenderFillRect(game->renderer, &dst_rect);
        }
    }

    for (i32 i = 0; i < game->entity_list.count; ++i) {
        Entity e = game->entity_list.e[i];

        if (e.type != NONE) {
            SDL_Rect dst_rect;

            dst_rect.x = e.position.x;
            dst_rect.y = e.position.y;
            dst_rect.w = e.size.w;
            dst_rect.h = e.size.h;

#if DEBUG_MODE
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

void GAME_SOUND_AND_DEBUG() {
    Mix_VolumeMusic(game->options.music_volume * MIX_MAX_VOLUME);

}
