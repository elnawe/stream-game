#ifndef PLAYER_H
#define PLAYER_H

#include "animation.h"
#include "raylib.h"

struct Player {
    // TODO: Animation deberia ser dinamico
    Animation idle_animation;
    Animation walk_animation;

    int speed;
    int max_speed;

    Texture2D sprite;

    Vector2 position;
    Vector2 velocity;
};

void player_init();
void player_handle_input(float delta_time);
void player_animate();
void player_update();
void player_render();

#endif
