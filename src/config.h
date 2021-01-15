#ifndef CONFIG_H
#define CONFIG_H

#define WINDOW_TITLE  "BoomerGame"

#if DEBUG_MODE
    #define SOURCE_DLL_NAME "build/game.dll"
    #define TEMP_DLL_NAME   "build/temp_game.dll"
#else
    #define SOURCE_DLL_NAME "game.dll"
    #define TEMP_DLL_NAME   "game.dll"
#endif

#define CONFIG_FILE_NAME      "run_tree/config.ini"
#define CONFIG_TEMP_FILE_NAME "run_tree/config.temp.ini"

// GRAPHIC WIDTH and HEIGHT are used to define the scale that the renderer will
// take to show an appropiate size of the 2D art in the game.
#define ORIGINAL_GRAPHIC_WIDTH  704
#define ORIGINAL_GRAPHIC_HEIGHT 352

// WINDOW WIDTH and HEIGHT are used to set the original state of the window
// (first as it opens).
#define WINDOW_WIDTH  1408
#define WINDOW_HEIGHT  704

// ASPECT_RATIO WIDTH and HEIGHT define the desired aspect ratio for the game.
#define ASPECT_RATIO_WIDTH  16
#define ASPECT_RATIO_HEIGHT  9

#endif
