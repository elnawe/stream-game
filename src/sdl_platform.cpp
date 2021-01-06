#define SDL_MAIN_HANDLED
#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include "SDL.h"
#include "config.h"
#include "game.h"

// DLL functions
typedef void (*game_init_func) (Game_Data *game);
typedef void (*game_loop_func) (Game_Data *game);

// Stub functions
void game_init_stub(Game_Data *it){}
void game_loop_stub(Game_Data *it){}

struct SDL_Game_Code {
    void *game_code_dll;

    FILETIME dll_file_time;
    const char *source_dll_name;
    const char *temp_dll_name;

    game_init_func game_init;
    game_loop_func game_loop;

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
    game_code.game_loop = game_loop_stub;
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

    // TODO: DLL file should be loaded from the same folder
    result.source_dll_name = "../build/game.dll";
    result.temp_dll_name = "../build/temp_game.dll";

    assert(SDL_platform_copy_file(result.source_dll_name, result.temp_dll_name));

    result.game_code_dll = SDL_LoadObject(result.temp_dll_name);

    if (result.game_code_dll == NULL) {
        printf("SDL_LoadObject failed: %s\n", SDL_GetError());
        exit(1);
    }

    result.game_init =
        (game_init_func) SDL_LoadFunction(result.game_code_dll, "game_init");
    result.game_loop =
        (game_loop_func) SDL_LoadFunction(result.game_code_dll, "game_loop");

    assert(result.game_init && result.game_loop);

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

void on_init(SDL_Renderer *renderer) {
    game_code = SDL_platform_load_game_code();

    assert(game_code.is_valid);
    game.screen_width = WINDOW_WIDTH;
    game.screen_height = WINDOW_HEIGHT;
    game.renderer = renderer;
    game_code.game_init(&game);
}

int main() {
    bool running = true;
    int frame_start, frame_time;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "[SDL] SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(WINDOW_TITLE,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          WINDOW_WIDTH,
                                          WINDOW_HEIGHT,
                                          SDL_WINDOW_OPENGL|
                                          SDL_WINDOW_SHOWN|
                                          SDL_WINDOW_RESIZABLE);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    on_init(renderer);

    while (running) {
        frame_start = SDL_GetTicks();

        // SDL_Event event;

        // while (SDL_PollEvent(&event)) {
        //     switch(event.type) {
        //         case SDL_QUIT:
        //         {
        //             running = false;
        //         } break;
        //         case SDL_KEYDOWN:
        //         {
        //             printf("KEY DOWN %d\n", event.key.keysym.sym);
        //             if (event.key.keysym.sym == SDLK_ESCAPE) {
        //                 running = false;
        //             }

        //             if (event.key.keysym.sym == SDLK_F5) {
        //                 SDL_platform_unload_game_code();

        //                 game_code =
        //                     SDL_platform_load_game_code("../build/game.dll",
        //                                                 "../build/game_temp.dll");

        //                 // Need this to refresh memory when the DLL is reloaded
        //                 game_code.game_init(&game);
        //             }
        //         }
        //     }
        // }

        if (game_code.is_valid) {
            game_code.game_loop(&game);
        }

        frame_time = SDL_GetTicks() - frame_start;

        if (frame_time < DELAY_TIME) {
            SDL_Delay(DELAY_TIME - frame_time);
        }

        if (SDL_platform_code_changed()) {
            SDL_platform_unload_game_code();

            game_code = SDL_platform_load_game_code();

            // Need this to refresh memory when the DLL is reloaded
            game_code.game_init(&game);
        }
    }

    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
