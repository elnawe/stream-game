#include "raylib.h"
#include "config.h"
#include "game.h"

#include "game.cpp"
#include "player.cpp"

#if DEBUG_MODE
#include "debug.h"
#include "debug.cpp"
#endif

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(GAME_FPS);

    init_game();

    while(!WindowShouldClose()) {
        handle_input();
        update_game();

        BeginDrawing();
        {
            ClearBackground(LIGHTGRAY);

            render_game();
        }

#if DEBUG_MODE
        render_debug_options();
#endif

        EndDrawing();
    }

    return 0;
}
