#ifndef FTW_H
#define FTW_H

#include "entity.h"
#include "item.h"
#include "SDL.h"
#include "texture_manager.h"

struct Game_Memory {
    bool is_initialized;

    u64 permanent_storage_size;
    void *permanent_storage; // NOTE: Required to be zero-ed out at startup

    u64 transient_storage_size;
    void *transient_storage; // NOTE: Required to be zero-ed out at startup
};

////////
////////
enum Game_Scene {
                 MENU,
                 GAME,
                 PAUSE
};

struct Game_State {
    const Uint8 *input;
    Vector2 player_position;
};

struct Game_Options {
    r32 music_volume;
};

struct Game_Data {
    float screen_width;
    float screen_height;
    float window_width;
    float window_height;
    bool running;
    float delta_time;
    SDL_Renderer *renderer;
    SDL_Window *window;

    Entity_List entity_list;
    Item_List item_list;
    Texture_Manager texture_manager;
    Game_Options options;
};

struct Game_Button_State {
    bool is_down;
};

struct Game_Input_Controller {
    union {
        Game_Button_State buttons[4];

        struct {
            Game_Button_State Y; // up - Triangle
            Game_Button_State A; // down - Cross
            Game_Button_State X; // left - Square
            Game_Button_State B; // right - Circle
        };
    };
};

struct Game_Input_Keyboard {
    // TODO: Add more buttons as we need them
    union {
        Game_Button_State buttons[4];

        struct {
            Game_Button_State move_up;
            Game_Button_State move_down;
            Game_Button_State move_left;
            Game_Button_State move_right;
        };
    };
};

struct Game_Input {
    // TODO: Implement controller
    Game_Input_Controller c;
    Game_Input_Keyboard kb;
};

// TODO: Should remove [..], Game_Data *game from here once we start
//       passing game data using the memory.
#define GAME_UPDATE_AND_RENDER(name) void name(Game_Memory *memory, Game_Input *input, Game_Data *game)
typedef GAME_UPDATE_AND_RENDER(Game_Update_And_Render);
GAME_UPDATE_AND_RENDER(game_update_and_render_stub){}

#endif
