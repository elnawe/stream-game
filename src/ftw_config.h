#ifndef FTW_CONFIG_H
#define FTW_CONFIG_H

#define WINDOW_TITLE  "FTW"

#if FTW_INTERNAL
    #define SOURCE_DLL_NAME "build/ftw.dll"
    #define TEMP_DLL_NAME   "build/ftw_temp.dll"
#else
    #define SOURCE_DLL_NAME "ftw.dll"
    #define TEMP_DLL_NAME   "ftw.dll"
#endif

#define CONFIG_FILE_NAME      "run_tree/config.ini"
#define CONFIG_TEMP_FILE_NAME "run_tree/config.temp.ini"

// If `true`, it will automatically resize the viewport every time
// the window is resized.
#define ENABLE_AUTO_RESIZE_VIEWPORT false

// GRAPHIC WIDTH and HEIGHT are used to define the scale that the renderer will
// take to show an appropiate size of the 2D art in the game.
#define ORIGINAL_GRAPHIC_WIDTH  640
#define ORIGINAL_GRAPHIC_HEIGHT 360

// WINDOW WIDTH and HEIGHT are used to set the original state of the window
// (first as it opens).
#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT  720

// ASPECT_RATIO WIDTH and HEIGHT define the desired aspect ratio for the game.
#define ASPECT_RATIO_WIDTH  16
#define ASPECT_RATIO_HEIGHT  9

// Dead zone of a left and right thumbstick controller
// https://bit.ly/3pkJhI2
#define XINPUT_DEAD_ZONE 8000

#endif
