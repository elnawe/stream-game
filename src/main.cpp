#include "./include/raylib.h"
#include <stdio.h>

const int width = 800;
const int height = 600;

void update_camera(Camera2D *camera, Vector2 player_position) {
    Vector2 camera_offset = { width / 2, height / 2 };

    camera->offset = camera_offset;
    camera->target = player_position;
}

int main() {
    InitWindow(width, height, "BoomerGame");

    Texture2D player = LoadTexture("pj.png");

    SetTargetFPS(60);

    Vector2 player_position = {0, 0};
    Vector2 player_velocity = {0, 0};
    int player_horizontal_speed = 30;
    int player_vertical_speed = 50;
    int max_vertical_speed = 15;
    int max_speed = 7;
    float gravity = 400.0f;

    int frameTotal = 3;
    float frameWidth = (float)(player.width / frameTotal);

    float animation_timer = 0.0f;
    float delta_time = 0.0f;
    int frame = 0;

    // TODO: Camara
    Vector2 camera_offset = { width / 2, height / 2 };

    Camera2D camera = {};
    camera.target = player_position;
    camera.offset = camera_offset;
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    bool player_can_jump = false;
    bool player_is_jumping = false;
    bool is_animated = false;

    while(!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(LIGHTGRAY);

        BeginMode2D(camera);

        update_camera(&camera, player_position);

        animation_timer += GetFrameTime();
        delta_time = GetFrameTime();

        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            if (player_velocity.x < max_speed) {
                player_velocity.x += player_horizontal_speed * delta_time;
            }

            player_position.x += player_velocity.x;
            is_animated = true;
        }
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            if (player_velocity.x < max_speed) {
                player_velocity.x += player_horizontal_speed * delta_time;
            }

            player_position.x -= player_velocity.x;
            is_animated = true;
        }

        // TODO: Cambiar por calculo de colisiones una vez que se empiece
        //       a programar el nivel.
        if (player_position.y < 498) {
            if (player_velocity.y == 0) {
                player_position.y += gravity * delta_time;
            }
        } else {
            player_can_jump = true;
            player_is_jumping = false;
        }

        if (player_can_jump && (IsKeyDown(KEY_W) || IsKeyDown(KEY_SPACE))) {
            player_can_jump = false;
            player_is_jumping = true;
            is_animated = true;
        }

        if (player_is_jumping) {
            if (player_velocity.y < max_vertical_speed) {
                player_velocity.y += player_vertical_speed * delta_time;
            } else {
                player_velocity.y = 0;
                player_is_jumping = false;
            }

            printf("%f\n", player_velocity.y);

            player_position.y -= player_velocity.y;
        }

        if (IsKeyUp(KEY_A) && IsKeyUp(KEY_D)) {
            is_animated = false;
        }

        if (IsKeyPressed(KEY_F)) {
            printf("Position: x:%d y:%d\n",
                   (int)player_position.x,
                   (int)player_position.y);
        }

        if (animation_timer >= 0.1f) {
            animation_timer = 0.0f;

            if (is_animated) {
                frame += 1;
            } else {
                frame = 0;
            }
        }

        frame = frame % 3;

        DrawTextureRec(player,
                       Rectangle{(frame * frameWidth), 0, 128, 128},
                       player_position,
                       RAYWHITE);

        DrawRectangle(300, 478, 300, 20, BLUE);

        EndMode2D();

        DrawFPS(0, 0);

        EndDrawing();
    }

    return 0;
}
