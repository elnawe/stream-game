#include "include/raylib.h"
#include "config.h"
#include "debug.h"
#include "game.h"
#include "player.h"

// TODO: Este codigo es temporalmente global y local pero tiene que
//       inicializarse en 'player.cpp'
static Player player = {};
static Camera2D camera = {};
static float animation_timer = 0.0f;
static Vector2 camera_offset = { WINDOW_WIDTH / 2, WINDOW_HEIGHT / 3 };

// TODO: Camera.cpp
void update_camera(Camera2D *camera, Vector2 player_position) {
    camera->offset = camera_offset;
    camera->target = player_position;
}

void init_game() {
    // TODO: Esta animacion esta half-baked.
    Animation walk_animation = {};
    walk_animation.name = "walk";
    walk_animation.frame_width = 128.0f;
    walk_animation.total_frames = 3;

    // TODO: Este sprite es temporal. Danny haceme un personajito <3 :>
    player.animation = walk_animation;
    player.speed = 30;
    player.max_speed = 7;
    player.texture = LoadTexture("pj.png");
    player.position = {0, 0};
    player.velocity = {0, 0};

    // TODO: Esto tiene que moverse a camera.cpp
    camera.target = player.position;
    camera.offset = camera_offset;
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void handle_input() {
    float delta_time = GetFrameTime();
    animation_timer += delta_time;

    // TODO: Deberiamos configurar el input en un config file (input.h)
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        if (player.velocity.x < player.max_speed) {
            player.velocity.x += player.speed * delta_time;
        }

        player.position.x += player.velocity.x;
        player.animation.is_running = true;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        if (player.velocity.x < player.max_speed) {
            player.velocity.x += player.speed * delta_time;
        }

        player.position.x -= player.velocity.x;
        player.animation.is_running = true;
    }
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        if (player.velocity.y < player.max_speed) {
            player.velocity.y += player.speed * delta_time;
        }

        player.position.y -= player.velocity.y;
        player.animation.is_running = true;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        if (player.velocity.y < player.max_speed) {
            player.velocity.y += player.speed * delta_time;
        }

        player.position.y += player.velocity.y;
        player.animation.is_running = true;
    }

    if (IsKeyUp(KEY_D) && IsKeyUp(KEY_RIGHT) &&
        IsKeyUp(KEY_A) && IsKeyUp(KEY_LEFT)) {
        player.velocity.x = 0;
    }
    if (IsKeyUp(KEY_W) && IsKeyUp(KEY_UP) &&
        IsKeyUp(KEY_S) && IsKeyUp(KEY_DOWN)) {
        player.velocity.y = 0;
    }

    if (player.velocity.x == 0 && player.velocity.y == 0) {
        player.animation.is_running = false;
    }

    if (animation_timer >= 0.2f) {
        debug_print("timer is off\n");
        animation_timer = 0.0f;

        if (player.animation.is_running) {
            debug_print("ahora si anime\n");
            player.animation.current_frame += 1;
        } else {
            player.animation.current_frame = 0;
        }
    }

    player.animation.current_frame = player.animation.current_frame % player.animation.total_frames;
}

void update_game() {
    update_camera(&camera, player.position);
}

void render_game() {
    render_debug_options();

    BeginMode2D(camera);

    DrawTextureRec(player.texture,
                   Rectangle{
                       player.animation.current_frame * player.animation.frame_width,
                           0,
                           128,
                           128},
                   player.position,
                   RAYWHITE);

    DrawRectangle(100, 100, 300, 20, BLUE);

    EndMode2D();
}
