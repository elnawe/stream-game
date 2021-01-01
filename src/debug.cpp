#include "raylib.h"
#include "config.h"
#include <stdio.h>

void debug_print(char *debug_text) {
    printf(debug_text);
}

void render_debug_options() {
    if (DEBUG_MODE) {
        Color debug_color = LIME;

        DrawText("FPS:", 0, 0, 20, debug_color);
        DrawFPS(110, 0);
        DrawText("Frametime:", 0, 20, 20, debug_color);
        DrawText(TextFormat("%f", GetFrameTime()), 110, 20, 20, debug_color);
    }
}
