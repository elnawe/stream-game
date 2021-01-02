#ifndef ANIMATION_H
#define ANIMATION_H

struct Animation {
    bool is_running;

    const char *name;

    Vector2 current_frame;

    int total_frames;

    Texture2D sprite;

    float frame_width;
    float frame_height;
};

#endif
