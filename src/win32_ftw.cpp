#define SDL_MAIN_HANDLED
#include "win32_ftw.h"

#include "ftw.h"
#include "ftw_config.h"
#include "ftw_intrinsics.h"

#include <stdio.h>
#include <windows.h>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

// External dependencies
#include "../external/ini.h"
#include "../external/ini.c"

global_var bool game_running = false;
global_var bool global_pause = false;

static SDL_Game_Config game_config = {};
static SDL_Game_Code game_code = {};
Game_Data game;

void SDL_platform_unload_game_code() {
    if (game_code.game_code_dll) {
        SDL_UnloadObject(game_code.game_code_dll);
        game_code.game_code_dll = NULL;
    }

    game_code.is_valid = false;
    game_code.game_update_and_render = game_update_and_render_stub;
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

#if FTW_INTERNAL
    assert(SDL_platform_copy_file(result.source_dll_name, result.temp_dll_name));
#endif

    result.game_code_dll = SDL_LoadObject(result.temp_dll_name);

    if (result.game_code_dll == NULL) {
        printf("SDL_LoadObject failed: %s\n", SDL_GetError());
        exit(1);
    }

    result.game_update_and_render =
        (Game_Update_And_Render *) SDL_LoadFunction(result.game_code_dll,
                                                    "game_update_and_render");

    assert(result.game_update_and_render);

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
    Game_Options *p_options = (Game_Options *)user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("Options", "music_volume")) {
        p_options->music_volume = atof(value);
    } else {
        return 0;
    }

    return 1;
}

Game_Options SDL_platform_load_config() {
    Game_Options result = {};
    WIN32_FILE_ATTRIBUTE_DATA file_data;
    char *ini_file = CONFIG_FILE_NAME;

#if FTW_INTERNAL
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

// TODO: Remove this. The Game_Memory should handle initialization
void on_init(SDL_Window *window, SDL_Renderer *renderer) {
    game_code = SDL_platform_load_game_code();

    assert(game_code.is_valid);
    game.options = SDL_platform_load_config();
    game.window_width = WINDOW_WIDTH;
    game.window_height = WINDOW_HEIGHT;
    game.window = window;
    game.renderer = renderer;
}

internal SDL_GameController *
register_controller() {
    SDL_GameController *result;

    for (int controller_index = 0;
         controller_index < SDL_NumJoysticks();
         ++controller_index) {
        if (SDL_IsGameController(controller_index)) {
            result = SDL_GameControllerOpen(controller_index);

            if (result) {
                printf("Game Controller plugged in slot %d\n",
                       controller_index);
                break;
            } else {
                printf("Error registering controller: %d.\n%s\n",
                       controller_index,
                       SDL_GetError());
            }
        }
    }

    return result;
}

internal void
controller_axis(SDL_GameController *controller,
                Vector2 *axis,
                SDL_GameControllerAxis axis_x,
                SDL_GameControllerAxis axis_y) {
    i16 MAX_MAGNITUDE = 32767;

    f32 X = (f32)SDL_GameControllerGetAxis(controller, axis_x);
    f32 Y = (f32)SDL_GameControllerGetAxis(controller, axis_y);
    f32 magnitude = sqrt(X*X + Y*Y);

    f32 normalized_X = X / magnitude;
    f32 normalized_Y = Y / magnitude;

    f32 normalized_magnitude = 0.0f;

    if (magnitude > XINPUT_DEAD_ZONE) {
        if (magnitude > MAX_MAGNITUDE) {
            magnitude = MAX_MAGNITUDE;
        }

        magnitude -= XINPUT_DEAD_ZONE;

        normalized_magnitude = magnitude / (MAX_MAGNITUDE - XINPUT_DEAD_ZONE);
    } else {
        magnitude = 0.0f;
        normalized_magnitude = 0.0f;
    }

    axis->x = (X*normalized_magnitude) / MAX_MAGNITUDE;
    axis->y = (Y*normalized_magnitude) / MAX_MAGNITUDE;
}


internal void
controller_button(SDL_GameController *controller,
                  Game_Button_State *input_state,
                  SDL_GameControllerButton button) {
    input_state->is_down = (SDL_GameControllerGetButton(controller, button) == 1);
}

internal void
keyboard_button(Game_Button_State *state,
                SDL_Scancode key) {
    bool is_down = (SDL_GetKeyboardState(NULL)[key] == 1);

    state->is_down = is_down;
}

int main() {
    u64 START, END;
    u32 fps_value = 0;
    r32 delta_time_value = 0;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER) != 0) {
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

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

#if ENABLE_AUTO_RESIZE_VIEWPORT
    // Recalculate renderer scale
    SDL_RenderSetScale(renderer,
                       (WINDOW_WIDTH  / ORIGINAL_GRAPHIC_WIDTH),
                       (WINDOW_HEIGHT / ORIGINAL_GRAPHIC_HEIGHT));
#endif

    on_init(window, renderer);

    if (window) {
        game_running = true;

        SDL_GameController *game_controller;
        Game_Input input[2];
        Game_Input *old_input = &input[0];
        Game_Input *new_input = &input[1];

#if FTW_INTERNAL
        LPVOID base_address = (LPVOID)Terabytes(2);
#else
        LPVOID base_address = 0;
#endif

        Game_Memory game_memory = {};
        game_memory.permanent_storage_size = Megabytes(64);
        game_memory.transient_storage_size = Gigabytes(4);

        u64 total_size = game_memory.permanent_storage_size +
            game_memory.transient_storage_size;

        game_memory.permanent_storage = VirtualAlloc(base_address,
                                                     total_size,
                                                     MEM_RESERVE|MEM_COMMIT,
                                                     PAGE_READWRITE);
        game_memory.transient_storage =
            ((u8 *)game_memory.permanent_storage + game_memory.permanent_storage_size);

        if (game_memory.permanent_storage && game_memory.transient_storage) {
            while (game_running) {
                if (game_code.is_valid) {
                    START = SDL_GetPerformanceCounter();

                                        while (SDL_PollEvent(&event)) {
                        switch(event.type) {
                            case SDL_QUIT: {
                                game_running = false;
                            } break;
                            case SDL_CONTROLLERDEVICEADDED: {
                                game_controller = register_controller();
                            } break;
                            case SDL_CONTROLLERDEVICEREMOVED: {
                                SDL_GameControllerClose(game_controller);
                                game_controller = NULL;
                            } break;
                            case SDL_WINDOWEVENT: {
                                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                                    r32 new_width = event.window.data1;
                                    r32 new_height = event.window.data2;
                                    r32 aspect_ratio_result;

#if ENABLE_AUTO_RESIZE_VIEWPORT
                                    // Keep aspect ratio defined in
                                    // ASPECT_RATIO_WIDTH & ASPECT_RATIO_HEIGHT
                                    if (new_width != game.window_width) {
                                        aspect_ratio_result =
                                            new_width / ASPECT_RATIO_WIDTH;
                                        new_height =
                                            aspect_ratio_result * ASPECT_RATIO_HEIGHT;
                                    } else if (new_height != game.window_height) {
                                        aspect_ratio_result =
                                            new_height / ASPECT_RATIO_HEIGHT;
                                        new_width =
                                            aspect_ratio_result * ASPECT_RATIO_WIDTH;
                                    }

                                    u32 new_width_ratio =
                                        (new_width  / ORIGINAL_GRAPHIC_WIDTH);
                                    u32 new_height_ratio =
                                        (new_height  / ORIGINAL_GRAPHIC_HEIGHT);

                                    SDL_RenderSetScale(game.renderer,
                                                       new_width_ratio,
                                                       new_height_ratio);
#endif

                                    // Resizing window and game scale
                                    SDL_SetWindowSize(game.window,
                                                      new_width,
                                                      new_height);

                                    game.window_width = new_width;
                                    game.window_height = new_height;
                                }
                            } break;
                            case SDL_KEYDOWN: {
#if FTW_INTERNAL
                                SDL_Keycode key_code = event.key.keysym.sym;

                                if (key_code == SDLK_ESCAPE) {
                                    game_running = false;
                                }

                                if (key_code == SDLK_p) {
                                    global_pause = !global_pause;
                                }
#endif
                            } break;
                        }
                    }

                    // Keyboard Input
                    {
                        Game_Input_Controller *keyboard = &new_input->controllers[0];

                        keyboard->is_analog = false;
                        // TODO: Can the player disable the keyboard input?
                        keyboard->is_enabled = true;

                        keyboard_button(&keyboard->move_up, SDL_SCANCODE_W);
                        keyboard_button(&keyboard->move_down, SDL_SCANCODE_S);
                        keyboard_button(&keyboard->move_left, SDL_SCANCODE_A);
                        keyboard_button(&keyboard->move_right, SDL_SCANCODE_D);

                        keyboard_button(&keyboard->left_shoulder, SDL_SCANCODE_Q);
                        keyboard_button(&keyboard->right_shoulder, SDL_SCANCODE_E);

                        keyboard_button(&keyboard->action_up, SDL_SCANCODE_I);
                        keyboard_button(&keyboard->action_down, SDL_SCANCODE_K);
                        keyboard_button(&keyboard->action_down, SDL_SCANCODE_LSHIFT);
                        keyboard_button(&keyboard->action_left, SDL_SCANCODE_J);
                        keyboard_button(&keyboard->action_right, SDL_SCANCODE_L);
                    }

                    // Controller Input
                    {
                        SDL_GameController *ctrl = game_controller;
                        Game_Input_Controller *controller = &new_input->controllers[1];

                        if (SDL_GameControllerGetAttached(ctrl)) {
                            controller->is_analog = true;
                            controller->is_enabled = true;

                            controller_axis(ctrl, &controller->left_axis,
                                            SDL_CONTROLLER_AXIS_LEFTX,
                                            SDL_CONTROLLER_AXIS_LEFTY);

                            controller_axis(ctrl, &controller->right_axis,
                                            SDL_CONTROLLER_AXIS_RIGHTX,
                                            SDL_CONTROLLER_AXIS_RIGHTY);

                            controller_button(ctrl, &controller->move_up,
                                              SDL_CONTROLLER_BUTTON_DPAD_UP);
                            controller_button(ctrl, &controller->move_down,
                                              SDL_CONTROLLER_BUTTON_DPAD_DOWN);
                            controller_button(ctrl, &controller->move_left,
                                              SDL_CONTROLLER_BUTTON_DPAD_LEFT);
                            controller_button(ctrl, &controller->move_right,
                                              SDL_CONTROLLER_BUTTON_DPAD_RIGHT);

                            controller_button(ctrl, &controller->left_shoulder,
                                              SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
                            controller_button(ctrl, &controller->right_shoulder,
                                              SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);

                            controller_button(ctrl, &controller->action_up,
                                              SDL_CONTROLLER_BUTTON_Y);
                            controller_button(ctrl, &controller->action_down,
                                              SDL_CONTROLLER_BUTTON_A);
                            controller_button(ctrl, &controller->action_left,
                                              SDL_CONTROLLER_BUTTON_X);
                            controller_button(ctrl, &controller->action_right,
                                              SDL_CONTROLLER_BUTTON_X);
                        } else {
                            controller->is_enabled = false;
                        }
                    }

                    if (!global_pause) {
                        SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
                        SDL_RenderClear(game.renderer);

                        game_code.game_update_and_render(&game_memory, new_input, &game);

                        SDL_RenderPresent(game.renderer);

                        r32 FREQ = (r32)SDL_GetPerformanceFrequency();
                        END = SDL_GetPerformanceCounter();
                        delta_time_value = (END - START) / FREQ;
                        fps_value = (u32)(1.0f / delta_time_value);
                        game.delta_time = delta_time_value;

                        old_input = new_input;
                    }
#if FTW_INTERNAL
                    char window_title_buffer[50];
                    sprintf(window_title_buffer,
                            "%s   FPS: %d   FRAMETIME: %f",
                            WINDOW_TITLE,
                            fps_value,
                            (delta_time_value * 1000.0f));
                    SDL_SetWindowTitle(window, window_title_buffer);
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
            }
        }
    }

    SDL_platform_unload_game_code();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
