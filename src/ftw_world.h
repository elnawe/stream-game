#ifndef FTW_WORLD_H
#define FTW_WORLD_H

enum WorldGen {
    //WorldGen_NONE,
    WorldGen_North,
    WorldGen_East,
    WorldGen_South,
    WorldGen_West
};

struct Room_Exit {
    u32 x;
    u32 y;
    WorldGen direction;
};

struct Room {
    u32 id;
    u32 *tiles;

    u32 colorR;
    u32 colorG;
    u32 colorB;

    Room_Exit exit[4];
};

struct Tilemap {
    r32 tile_side_in_meters;
    r32 tile_side_in_pixels;

    u32 tilemap_count;

    Room *rooms;

    u32 count_x;
    u32 count_y;
};

struct World {
    Tilemap *tilemap;
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

#endif
