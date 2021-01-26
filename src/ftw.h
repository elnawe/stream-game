#ifndef FTW_H
#define FTW_H

#include "entity.h"
#include "item.h"
#include "SDL.h"
#include "texture_manager.h"

#include "ftw_world.h"

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

struct Memory_Arena {
    mem_size size;
    u8 *base;
    mem_size used;
};

struct Game_State {
    Memory_Arena world_arena;
    World *world;

    World_Position player_p;
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
    bool is_analog;

    Vector2 left_axis;
    Vector2 right_axis;

    union {
        Game_Button_State buttons[12];

        struct {
            // Directional Pad
            Game_Button_State move_up;
            Game_Button_State move_down;
            Game_Button_State move_left;
            Game_Button_State move_right;

            Game_Button_State left_shoulder;
            Game_Button_State right_shoulder;

            // TODO: Figure out why SDL doesn't support LB and RB
            // PlayStation: L1 | Xbox: LB
            // Game_Button_State left_button;
            // PlayStation: R1 | Xbox: RB
            // Game_Button_State right_button;

            // PlayStation: Triangle | Xbox: Y
            Game_Button_State action_up;

            // PlaySTation: Cross | Xbox: A
            Game_Button_State action_down;

            // PlayStation: Square | Xbox: X
            Game_Button_State action_left;

            // PlayStation: Circle | Xbox: B
            Game_Button_State action_right;

        };
    };
};

struct Game_Input {
    Game_Input_Controller controllers[4];
};

// TODO: Should remove [..], Game_Data *game from here once we start
//       passing game data using the memory.
#define GAME_UPDATE_AND_RENDER(name) void name(Game_Memory *memory, Game_Input *input, Game_Data *game)
typedef GAME_UPDATE_AND_RENDER(Game_Update_And_Render);
GAME_UPDATE_AND_RENDER(game_update_and_render_stub){}

#endif
