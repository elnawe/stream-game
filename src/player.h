#ifndef PLAYER_H
#define PLAYER_H

struct Animation {
    bool is_running;

    char *name;

    int current_frame;
    int total_frames;

    float frame_width;
};

struct Player {
    Animation animation;

    int speed;
    int max_speed;

    Texture2D texture;

    Vector2 position;
    Vector2 velocity;
};

#endif
