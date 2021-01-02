#include "animation.h"
#include "player.h"
#include "raylib.h"
#include "config.h"

static Player player = {};

void player_init() {
    Texture2D player_sprite = LoadTexture("pj.png");
    float middle_of_screen = (WINDOW_HEIGHT - 128) / 2;

    // TODO: Esta animacion esta half-baked.
    Animation idle_animation = {};
    idle_animation.name = "idle";
    idle_animation.frame_width = 128.0f;
    idle_animation.frame_height = 128.0f;
    idle_animation.sprite = player_sprite;
    idle_animation.total_frames = 2;
    idle_animation.current_frame = {0, 0};

    // TODO y esta tambien :^D
    Animation walk_animation = {};
    walk_animation.name = "walk";
    walk_animation.frame_width = 128.0f;
    walk_animation.frame_height = 128.0f;
    walk_animation.sprite = player_sprite;
    walk_animation.total_frames = 3;
    walk_animation.current_frame = {0, 1};

    player.idle_animation = idle_animation;
    player.walk_animation = walk_animation;
    player.speed = 30;
    player.max_speed = 7;
    player.sprite = player_sprite;
    player.position = {0, middle_of_screen};
    player.velocity = {0, 0};
}

void player_handle_input(float delta_time) {
    // TODO: Deberiamos configurar el input en un config file (input.h)
    float move_to_x = GetGamepadAxisMovement(0, 0);
    float move_to_y = GetGamepadAxisMovement(0, 1);

    if (IsGamepadAvailable(0)) {
        if (move_to_x > 0) {
            if (player.velocity.x < player.max_speed) {
                player.velocity.x += player.speed * delta_time;
            }
            player.position.x += player.velocity.x;
            player.walk_animation.is_running = true;
        } else if (move_to_x < 0) {
            if (player.velocity.x < player.max_speed) {
                player.velocity.x += player.speed * delta_time;
            }
            player.position.x -= player.velocity.x;
            player.walk_animation.is_running = true;
        }

        if (move_to_y > 0) {
            if (player.velocity.y < player.max_speed) {
                player.velocity.y += player.speed * delta_time;
            }
            player.position.y += player.velocity.y;
            player.walk_animation.is_running = true;
        } else if (move_to_y < 0) {
            if (player.velocity.y < player.max_speed) {
                player.velocity.y += player.speed * delta_time;
            }
            player.position.y -= player.velocity.y;
            player.walk_animation.is_running = true;
        }
    } else {
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            if (player.velocity.x < player.max_speed) {
                player.velocity.x += player.speed * delta_time;
            }

            player.position.x += player.velocity.x;
            player.walk_animation.is_running = true;
        }
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            if (player.velocity.x < player.max_speed) {
                player.velocity.x += player.speed * delta_time;
            }

            player.position.x -= player.velocity.x;
            player.walk_animation.is_running = true;
        }
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
            if (player.velocity.y < player.max_speed) {
                player.velocity.y += player.speed * delta_time;
            }

            player.position.y -= player.velocity.y;
            player.walk_animation.is_running = true;
        }
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
            if (player.velocity.y < player.max_speed) {
                player.velocity.y += player.speed * delta_time;
            }

            player.position.y += player.velocity.y;
            player.walk_animation.is_running = true;
        }

        if (IsKeyUp(KEY_D) && IsKeyUp(KEY_RIGHT) &&
            IsKeyUp(KEY_A) && IsKeyUp(KEY_LEFT)) {
            player.velocity.x = 0;
        }
        if (IsKeyUp(KEY_W) && IsKeyUp(KEY_UP) &&
            IsKeyUp(KEY_S) && IsKeyUp(KEY_DOWN)) {
            player.velocity.y = 0;
        }
    }

    if (player.velocity.x == 0 && player.velocity.y == 0) {
        player.walk_animation.is_running = false;
    }

    if (move_to_x == 0 && move_to_y == 0) {
        player.walk_animation.is_running = false;
    }
}

void player_animate() {
    if (player.walk_animation.is_running) {
        player.walk_animation.current_frame.x += 1;
    } else {
        player.walk_animation.current_frame.x = 0;

        player.idle_animation.current_frame.x += 1;
    }

    player.walk_animation.current_frame.x =
        (int)player.walk_animation.current_frame.x % player.walk_animation.total_frames;

    player.idle_animation.current_frame.x =
        (int)player.idle_animation.current_frame.x % player.idle_animation.total_frames;
}

void player_update() {

}

void player_render() {
    Animation current_animation;

    if (player.walk_animation.is_running) {
        current_animation = player.walk_animation;
    } else {
        current_animation = player.idle_animation;
    }

    Vector2 sprite_rect = {
        current_animation.current_frame.x * current_animation.frame_width,
        current_animation.current_frame.y * current_animation.frame_height
    };
    Rectangle player_src = {
        sprite_rect.x,
        sprite_rect.y,
        128,
        128
    };

    DrawTextureRec(player.sprite,
                   player_src,
                   player.position,
                   RAYWHITE);
}
