#include "raylib.h"
#include "config.h"
#include "game.h"
#include "player.h"

#include "debug.h"
// TODO: Este codigo es temporalmente global y local pero tiene que
//       inicializarse en 'player.cpp'
static float animation_timer = 0.0f;
static Texture2D yasuo = {};

void init_game() {
    player_init();

    yasuo = LoadTexture("yasuo.png");
}

void handle_input() {
    float delta_time = GetFrameTime();

    player_handle_input(delta_time);
}

void update_game() {
    animation_timer += GetFrameTime();

    if (animation_timer >= 0.3f) {
        // NOTE: Cada 18.75 frames, ocurre una animacion
        animation_timer = 0.0f;

        player_animate();
    }

    player_update();
}

void render_game() {
    player_render();

    DrawTextureRec(yasuo,
                   Rectangle{0, 0, 200, 200},
                   Vector2{200, 400},
                   RAYWHITE);

    DrawTextureRec(yasuo,
                   Rectangle{200, 0, 200, 200},
                   Vector2{400, 400},
                   RAYWHITE);

    DrawRectangle(150, 570, 500, 20, BLUE);
}
