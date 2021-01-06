#include <stdio.h>

#include "game.h"
#include "config.h"
#include "entity.h"

#include "entity.cpp"

// #include "raylib.h"
// #include "config.h"
// #include "game.h"
// #include "player.h"

static Entity test_entity;
static int frame_time;
static SDL_RendererFlip facing_direction = SDL_FLIP_NONE;
static bool go_left = false;

void game_init(Game_Data *game) {
    test_entity = create_entity("player.bmp", game->renderer);

    test_entity.dest_rect.w *= 4;
    test_entity.dest_rect.h *= 4;
}

void game_loop(Game_Data *game) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
            {
                exit(0);
            }
            case SDL_KEYDOWN:
            {
                printf("KEY DOWN %d\n", event.key.keysym.sym);
                if (event.key.keysym.sym == SDLK_s) {
                    test_entity.dest_rect.y -= 16;
                }
            }
        }
    }

    // TODO: Frame time is not true, the cake is a lie!!
    frame_time += 1;

    SDL_SetRenderDrawColor(game->renderer, 230, 230, 230, 255);
    SDL_RenderClear(game->renderer);

    test_entity.src_rect.w = 16;
    test_entity.src_rect.h = 16;
    test_entity.src_rect.y = 16;

    // TODO: FIX MEEEEEE!!!111
    if (frame_time > 50) {
        frame_time = 0;
        test_entity.src_rect.x += 16;

        if (test_entity.src_rect.x > 16) {
            test_entity.src_rect.x = 0;
        }

        if (test_entity.dest_rect.x >= 700) {
            go_left = true;
        } else if (test_entity.dest_rect.x <= 0) {
            go_left = false;
        }

        if (go_left) {
            test_entity.dest_rect.x -= 16;
            facing_direction = SDL_FLIP_HORIZONTAL;
        } else {
            test_entity.dest_rect.x += 16;
            facing_direction = SDL_FLIP_NONE;
        }
    }

    SDL_RenderCopyEx(game->renderer,
                     test_entity.texture,
                     &test_entity.src_rect,
                     &test_entity.dest_rect,
                     0, NULL, facing_direction);

    SDL_RenderPresent(game->renderer);
}

// #include "debug.h"
// // TODO: Este codigo es temporalmente global y local pero tiene que
// //       inicializarse en 'player.cpp'
// static float animation_timer = 0.0f;
// static Texture2D yasuo = {};

// void init_game() {
//     player_init();

//     yasuo = LoadTexture("yasuo.png");
// }

// void handle_input() {
//     float delta_time = GetFrameTime();

//     player_handle_input(delta_time);
// }

// void update_game() {
//     animation_timer += GetFrameTime();

//     if (animation_timer >= 0.3f) {
//         // NOTE: Cada 18.75 frames, ocurre una animacion
//         animation_timer = 0.0f;

//         player_animate();
//     }

//     player_update();
// }

// void render_game() {
//     player_render();

//     DrawTextureRec(yasuo,
//                    Rectangle{0, 0, 200, 200},
//                    Vector2{200, 400},
//                    RAYWHITE);

//     DrawTextureRec(yasuo,
//                    Rectangle{200, 0, 200, 200},
//                    Vector2{400, 400},
//                    RAYWHITE);

//     DrawRectangle(150, 570, 500, 20, BLUE);
// }
