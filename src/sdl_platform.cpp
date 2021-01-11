#define SDL_MAIN_HANDLED
#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#include "config.h"
#include "game.h"

#if DEBUG_MODE
#include <stdlib.h>
#include <string.h>
#endif

// DLL functions
typedef void (*game_init_func) (Game_Data *game);
typedef void (*game_handle_input_func) (Game_Data *game);
typedef void (*game_update_and_render_func) (Game_Data *game);
typedef void (*game_sound_and_debug_func) (Game_Data *game);
// Stub functions
void game_init_stub(Game_Data *it){}
void game_handle_input_stub(Game_Data *it){}
void game_update_and_render_stub(Game_Data *it){}
void game_sound_and_debug_stub(Game_Data *it){}

struct SDL_Game_Code {
    void *game_code_dll;

    FILETIME dll_file_time;
    const char *source_dll_name;
    const char *temp_dll_name;

    game_init_func game_init;
    game_handle_input_func game_handle_input;
    game_update_and_render_func game_update_and_render;
    game_sound_and_debug_func game_sound_and_debug;

    bool is_valid;
};

static SDL_Game_Code game_code = {};
Game_Data game;

void SDL_platform_unload_game_code() {
    if (game_code.game_code_dll) {
        SDL_UnloadObject(game_code.game_code_dll);
        game_code.game_code_dll = NULL;
    }

    game_code.is_valid = false;
    game_code.game_init = game_init_stub;
    game_code.game_handle_input = game_handle_input_stub;
    game_code.game_update_and_render = game_update_and_render_stub;
    game_code.game_sound_and_debug = game_sound_and_debug_stub;
    game_code.dll_file_time = {};
}

bool SDL_platform_copy_file(const char *source_dll_name,
                            const char *temp_dll_name) {
    SDL_RWops *source = SDL_RWFromFile(source_dll_name, "r");
    SDL_RWops *target = SDL_RWFromFile(temp_dll_name, "w");

    if (!source || !target) {
        return false;
    }

    // Read source into buffer
    Sint64 size = SDL_RWsize(source);

    void *buffer = SDL_calloc(1, size);
    SDL_RWread(source, buffer, size, 1);

    // Write buffer to target
    SDL_RWwrite(target, buffer, size, 1);

    printf("File copied successfully.\n");

    SDL_RWclose(source);
    SDL_RWclose(target);
    SDL_free(buffer);

    return true;
}

SDL_Game_Code SDL_platform_load_game_code() {
    SDL_Game_Code result = {};
    WIN32_FILE_ATTRIBUTE_DATA file_data;

#if DEBUG_MODE
    // TODO: DLL file should be loaded from the same folder
    result.source_dll_name = SOURCE_DLL_NAME;
        result.temp_dll_name = TEMP_DLL_NAME;

    assert(SDL_platform_copy_file(result.source_dll_name, result.temp_dll_name));
#else
    result.source_dll_name = SOURCE_DLL_NAME;
    result.temp_dll_name = SOURCE_DLL_NAME;
#endif

    result.game_code_dll = SDL_LoadObject(result.temp_dll_name);

    if (result.game_code_dll == NULL) {
        printf("SDL_LoadObject failed: %s\n", SDL_GetError());
        exit(1);
    }

    result.game_init =
        (game_init_func) SDL_LoadFunction(result.game_code_dll,
                                          "GAME_INIT");
    result.game_handle_input =
        (game_handle_input_func) SDL_LoadFunction(result.game_code_dll,
                                                  "GAME_HANDLE_INPUT");
    result.game_update_and_render =
        (game_update_and_render_func) SDL_LoadFunction(result.game_code_dll,
                                                       "GAME_UPDATE_AND_RENDER");
    result.game_sound_and_debug =
        (game_sound_and_debug_func) SDL_LoadFunction(result.game_code_dll,
                                                     "GAME_SOUND_AND_DEBUG");

    assert(result.game_init &&
           result.game_handle_input &&
           result.game_update_and_render &&
           result.game_sound_and_debug);

    result.is_valid = true;

    if (GetFileAttributesExA(result.source_dll_name,
                             GetFileExInfoStandard,
                             &file_data)) {
        result.dll_file_time = file_data.ftLastWriteTime;
    }

    return result;
}

bool SDL_platform_code_changed() {
    WIN32_FILE_ATTRIBUTE_DATA new_file_data;
    FILETIME new_file_time;

    if (GetFileAttributesExA(game_code.source_dll_name,
                             GetFileExInfoStandard,
                             &new_file_data)) {
        new_file_time = new_file_data.ftLastWriteTime;

        if (CompareFileTime(&new_file_time, &game_code.dll_file_time) != 0) {
            // Sleep lets Windows to close the file before opening it again
            // to do the copy once the change happens.
            Sleep(500);
            return true;
        }
    }

    return false;
}

void on_init(SDL_Window *window, SDL_Renderer *renderer) {
    game_code = SDL_platform_load_game_code();

    assert(game_code.is_valid);
    game.screen_width = ORIGINAL_GRAPHIC_WIDTH;
    game.screen_height = ORIGINAL_GRAPHIC_HEIGHT;
    game.window_width = WINDOW_WIDTH;
    game.window_height = WINDOW_HEIGHT;
    game.running = true;
    game.window = window;
    game.renderer = renderer;
    game_code.game_init(&game);
}

int main() {
    Uint64 START, END;
    SDL_Window *window;
    SDL_Renderer *renderer;

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
        fprintf(stderr, "[SDL] SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    if (Mix_Init(MIX_INIT_MP3) != MIX_INIT_MP3) {
        fprintf(stderr, "[SDL_mixer] Mix_Init Error: %s\n", Mix_GetError());
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) != 0) {
        printf("ERROR: Couldn't open audio.\n Message: %s\n", Mix_GetError());
        return 1;
    }

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        fprintf(stderr, "[SDL_image] IMG_Init Error: %s\n", IMG_GetError());
        return 1;
    }

    if (TTF_Init() == -1) {
        printf("[SDL_ttf] TTF_Init Error: %s\n", TTF_GetError());
        return 1;
    }

    window = SDL_CreateWindow(WINDOW_TITLE,
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH,
                              WINDOW_HEIGHT,
                              SDL_WINDOW_OPENGL|
                              SDL_WINDOW_SHOWN|
                              SDL_WINDOW_RESIZABLE);

    renderer = SDL_CreateRenderer(window, -1,
                                  SDL_RENDERER_ACCELERATED|
                                  SDL_RENDERER_PRESENTVSYNC);

    // Recalculate renderer scale
    SDL_RenderSetScale(renderer,
                       (WINDOW_WIDTH  / ORIGINAL_GRAPHIC_WIDTH),
                       (WINDOW_HEIGHT / ORIGINAL_GRAPHIC_HEIGHT));

    on_init(window, renderer);

    while (game.running && game_code.is_valid) {
        START = SDL_GetPerformanceCounter();

        game_code.game_handle_input(&game);

        SDL_SetRenderDrawColor(game.renderer, 230, 230, 230, 255);
        SDL_RenderClear(game.renderer);

        game_code.game_update_and_render(&game);

        SDL_RenderPresent(game.renderer);

        END = SDL_GetPerformanceCounter();
        float FREQ = (float)SDL_GetPerformanceFrequency();
        game.delta_time = ((END - START) * GAME_SPEED_MULTIPLIER) / FREQ;

        game_code.game_sound_and_debug(&game);

#if DEBUG_MODE
        if (SDL_platform_code_changed()) {
            SDL_platform_unload_game_code();

            game_code = SDL_platform_load_game_code();
        }
#endif
    }

    SDL_platform_unload_game_code();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
