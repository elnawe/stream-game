#include "raylib.h"
#include "config.h"
#include "game.h"

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(GAME_FPS);

    init_game();

    while(!WindowShouldClose()) {
        handle_input();
        update_game();

        BeginDrawing();
        ClearBackground(LIGHTGRAY);

        render_game();

        EndDrawing();
    }

    return 0;
}
