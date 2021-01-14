#define SDL_MAIN_HANDLED
#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include "config.h"
#include "game.h"
#include "ini.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

#if DEBUG_MODE
    #include <stdlib.h>
    #include <string.h>
#endif

#include "ini.c"

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

struct SDL_Game_Config {
    FILETIME last_update;
    bool is_valid;
};

static TTF_Font *press_start_font;
static SDL_Color text_color = {255, 255, 255};
static SDL_Game_Config game_config = {};
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

bool SDL_platform_copy_file(const char *source_file,
                            const char *temp_file) {
    SDL_RWops *source = SDL_RWFromFile(source_file, "r");
    SDL_RWops *target = SDL_RWFromFile(temp_file, "w");

    if (!source || !target) {
        return false;
    }

    // Read source into buffer
    Sint64 size = SDL_RWsize(source);

    void *buffer = SDL_calloc(1, size);
    SDL_RWread(source, buffer, size, 1);

    // Write buffer to target
    SDL_RWwrite(target, buffer, size, 1);

    printf("%s --> File copied successfully.\n", source_file);

    SDL_RWclose(source);
    SDL_RWclose(target);
    SDL_free(buffer);

    return true;
}

SDL_Game_Code SDL_platform_load_game_code() {
    SDL_Game_Code result = {};
    WIN32_FILE_ATTRIBUTE_DATA file_data;

    // TODO: DLL file should be loaded from the same folder
    result.source_dll_name = SOURCE_DLL_NAME;
    result.temp_dll_name = TEMP_DLL_NAME;

#if DEBUG_MODE
    assert(SDL_platform_copy_file(result.source_dll_name, result.temp_dll_name));
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

bool SDL_platform_config_changed() {
    WIN32_FILE_ATTRIBUTE_DATA file_data;

    if (GetFileAttributesExA(CONFIG_FILE_NAME,
                             GetFileExInfoStandard,
                             &file_data)) {
        FILETIME new_file_time = file_data.ftLastWriteTime;

        if (CompareFileTime(&new_file_time, &game_config.last_update) != 0) {
            // Sleep lets Windows to close the file before opening it again
            // to do the copy once the change happens.
            Sleep(500);
            return true;
        }
    }

    return false;
}

void SDL_platform_unload_config() {

}

int ini_parser_callback(void* user,
                        const char* section,
                        const char* name,
                        const char* value) {
    Game_Options *poptions = (Game_Options *)user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("Options", "music_volume")) {
        poptions->music_volume = atof(value);
    } else {
        return 0;
    }

    return 1;
}

Game_Options SDL_platform_load_config() {
    Game_Options result = {};
    WIN32_FILE_ATTRIBUTE_DATA file_data;
    char *ini_file = CONFIG_FILE_NAME;

#if DEBUG_MODE
    ini_file = CONFIG_TEMP_FILE_NAME;
#endif

    assert(SDL_platform_copy_file(CONFIG_FILE_NAME, CONFIG_TEMP_FILE_NAME));

    // Parse .ini file and set new variables
    if (ini_parse(ini_file, ini_parser_callback, &result) < 0) {
        printf("[sdl_platform] Error loading configuration file: %s\n", ini_file);
        exit(1);
    }

    game_config.is_valid = true;

    if (GetFileAttributesExA(CONFIG_FILE_NAME,
                             GetFileExInfoStandard,
                             &file_data)) {
        game_config.last_update = file_data.ftLastWriteTime;
    }

    return result;
}

void on_init(SDL_Window *window, SDL_Renderer *renderer) {
    game_code = SDL_platform_load_game_code();

    assert(game_code.is_valid);
    game.options = SDL_platform_load_config();
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
    u64 START, END;
    u32 fps_value = 0;
    r32 internal_delta_time_value = 0;
    SDL_Window *window;
    SDL_Renderer *renderer;

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
        fprintf(stderr, "[SDL] SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    if (Mix_Init(MIX_INIT_MP3) != MIX_INIT_MP3) {
        fprintf(stderr, "[SDL_mixer] Mix_Init Error: %s\n", Mix_GetError());
        return 1;
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

    // FONT
    press_start_font = TTF_OpenFont("assets/PressStart2P-Regular.ttf", 14);

    while (game.running) {
        START = SDL_GetPerformanceCounter();

        if (game_code.is_valid) {
            game_code.game_handle_input(&game);
        }

        SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
        SDL_RenderClear(game.renderer);

        if (game_code.is_valid) {
            game_code.game_update_and_render(&game);
        }

#if DEBUG_MODE
        // DEBUG
        SDL_Rect ft_rect, fps_rect;

        // Show FRAMETIME
        char ft_amount[12];

        ft_rect.x = 0;
        ft_rect.y = 0;
        ft_rect.w = 180;
        ft_rect.h = 15;

        sprintf(ft_amount, "FRAMETIME %f ms", (internal_delta_time_value * 1000));
        SDL_Surface *ft_surface = TTF_RenderText_Solid(press_start_font, ft_amount, text_color);
        SDL_Texture *ft_texture = SDL_CreateTextureFromSurface(game.renderer, ft_surface);
        SDL_FreeSurface(ft_surface);

        // Show FPS
        char fps_amount[12];

        fps_rect.x = ft_rect.x;
        fps_rect.y = ft_rect.y + ft_rect.h + 5;
        fps_rect.w = 0.3f * ft_rect.w;
        fps_rect.h = 0.5f * ft_rect.h;

        sprintf(fps_amount, "FPS %d", fps_value);
        SDL_Surface *fps_surface = TTF_RenderText_Solid(press_start_font, fps_amount, text_color);
        SDL_Texture *fps_texture = SDL_CreateTextureFromSurface(game.renderer, fps_surface);
        SDL_FreeSurface(fps_surface);


        SDL_RenderCopy(game.renderer, ft_texture, NULL, &ft_rect);
        SDL_RenderCopy(game.renderer, fps_texture, NULL, &fps_rect);
#endif

        SDL_RenderPresent(game.renderer);

        r32 FREQ = (r32)SDL_GetPerformanceFrequency();
        END = SDL_GetPerformanceCounter();
        game.delta_time = ((END - START) * GAME_SPEED_MULTIPLIER) / FREQ;

#if DEBUG_MODE
        fps_value = (u32)(1.0f / ((END-START) / FREQ));
        internal_delta_time_value = (END - START) / FREQ;
#endif

        if (game_code.is_valid) {
            game_code.game_sound_and_debug(&game);
        }

#if DEBUG_MODE
        if (SDL_platform_code_changed()) {
            SDL_platform_unload_game_code();

            game_code = SDL_platform_load_game_code();
        }

        if (SDL_platform_config_changed()) {
            SDL_platform_unload_config();

            game.options = SDL_platform_load_config();
        }
#endif
    }

    SDL_platform_unload_game_code();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
