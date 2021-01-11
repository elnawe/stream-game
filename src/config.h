#ifndef CONFIG_H
#define CONFIG_H

#if DEBUG_MODE
  #define SOURCE_DLL_NAME "build/game.dll"
  #define TEMP_DLL_NAME "build/temp_game.dll"
#else
  #define SOURCE_DLL_NAME "game.dll"
  #define TEMP_DLL_NAME "game.dll"
#endif

#define WINDOW_TITLE  "BoomerGame"

// Defines the multiplier of dT. See following table for examples.
// |  VALUE  | VELOCITY |
// |---------|----------|
// |   1.0f  |      100 |
// |  10.0f  |       10 |
// | 100.0f  |        1 |
#define GAME_SPEED_MULTIPLIER 10.0f

// GRAPHIC WIDTH and HEIGHT are used to define the scale that the renderer will
// take to show an appropiate size of the 2D art in the game.
#define ORIGINAL_GRAPHIC_WIDTH  640
#define ORIGINAL_GRAPHIC_HEIGHT 360

// WINDOW WIDTH and HEIGHT are used to set the original state of the window
// (first as it opens).
#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720

// ASPECT_RATIO WIDTH and HEIGHT define the desired aspect ratio for the game.
#define ASPECT_RATIO_WIDTH  16
#define ASPECT_RATIO_HEIGHT  9

#endif
