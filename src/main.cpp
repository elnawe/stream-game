#include "./include/raylib.h"
#include <stdio.h>

int main() {
    const int width = 800;
    const int height = 600;

    InitWindow(width, height, "BoomerGame");

    Texture2D nawelito = LoadTexture("../data/pj.png");

    SetTargetFPS(60);

    Vector2 nawelito_position = {0, 0};

    int frameTotal = 3;
    float frameWidth = (float)(nawelito.width / frameTotal);

    float timer = 0.0f;
    int frame = 0;

    bool isAnimated = false;

    while(!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(LIGHTGRAY);

        DrawFPS(0, 0);

        timer += GetFrameTime();

        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
            nawelito_position.y += 3;
            isAnimated = true;
        }

        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
            nawelito_position.y -= 3;
            isAnimated = true;
        }
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            nawelito_position.x += 3;
            isAnimated = true;
        }
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            nawelito_position.x -= 3;
            isAnimated = true;
        }

        if (IsKeyUp(KEY_W) && IsKeyUp(KEY_A) && IsKeyUp(KEY_S) && IsKeyUp(KEY_D)) {
            isAnimated = false;
        }

        if (IsKeyPressed(KEY_SPACE)) {
            printf("Position: x:%d y:%d\n",
                   (int)nawelito_position.x,
                   (int)nawelito_position.y);
        }

        if (timer >= 0.2f) {
            timer = 0.0f;

            if (isAnimated) {
                frame += 1;
            } else {
                frame = 0;
            }
        }

        frame = frame % 3;

        DrawTextureRec(nawelito,
                       Rectangle{(frame * frameWidth), 0, 128, 128},
                       nawelito_position,
                       RAYWHITE);

        EndDrawing();
    }

    return 0;
}
