#ifndef FTW_WORLD_H
#define FTW_WORLD_H

enum FloorGenMove {
    FloorGenMove_Up,
    FloorGenMove_Right,
    FloorGenMove_Down,
    FloorGenMove_Left,
};

struct Floor {
    u32 count_x;
    u32 count_y;
    u32 discovered;

    u32 max_rooms;
    u32 min_rooms;
    u32 total_rooms;

    u32 *rooms;
};

struct Tilemap {
    u32 colorR;
    u32 colorG;
    u32 colorB;

    u32 *tiles;
};

struct World {
    r32 tile_side_meters;
    s32 tile_side_pixels;
    r32 meters_to_pixels;

    u32 tile_count_x;
    u32 tile_count_y;

    u32 count_x;
    u32 count_y;

    r32 offset_x;
    r32 offset_y;

    Tilemap *tilemaps;
};

struct World_Position {
    s32 tilemap_x;
    s32 tilemap_y;

    s32 tile_x;
    s32 tile_y;

    // NOTE: X and Y relative to the tile
    //       (top-left corner of the tile)
    // Math friendly, resolution independent of
    // world units relative to a tile.
    r32 tile_relative_x;
    r32 tile_relative_y;
};

// TODO: two-dimension array of world
// u32 floor_schema[FLOOR_SIZE][FLOOR_SIZE] =


#endif
