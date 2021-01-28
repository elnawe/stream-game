#include "ftw_world.h"

global_var const u32 TILEMAP_SIZE_Y = 9;
global_var const u32 TILEMAP_SIZE_X = 17;

global_var u32 room_nesw00[TILEMAP_SIZE_Y][TILEMAP_SIZE_X] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

global_var u32 room_nesw01[TILEMAP_SIZE_Y][TILEMAP_SIZE_X] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

global_var u32 room_nesw02[TILEMAP_SIZE_Y][TILEMAP_SIZE_X] = {
    {9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9},
    {9, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 9},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {9, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 9},
    {9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9}
};

global_var u32 room_nesw03[TILEMAP_SIZE_Y][TILEMAP_SIZE_X] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

global_var u32 room_nesw04[TILEMAP_SIZE_Y][TILEMAP_SIZE_X] = {
    {9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9},
    {9, 9, 9, 9, 9, 9, 9, 1, 0, 1, 9, 9, 9, 9, 9, 9, 9},
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
    {9, 9, 9, 9, 9, 9, 9, 1, 0, 1, 9, 9, 9, 9, 9, 9, 9},
    {9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9}
};

global_var u32 room_nesw05[18][TILEMAP_SIZE_X] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

internal void
set_room(Tilemap *tilemap, Room *room, u32 room_seed) {
    u32 *available_tilemaps[5] = {
        (u32 *)room_nesw00,
        (u32 *)room_nesw01,
        (u32 *)room_nesw02,
        (u32 *)room_nesw03,
        (u32 *)room_nesw04
    };
    u32 *tileset = available_tilemaps[room_seed];

    for (int y = 0; y < tilemap->count_y; ++y) {
        for (int x = 0; x < tilemap->count_x; ++x) {
            u32 tile_value = y*tilemap->count_x + x;

            room->tiles[tile_value] = tileset[tile_value];
        }
    }

}

internal void
normalize_coords(u32 tile_count, s32 *tilemap_pos, s32 *tile_pos) {
    while (*tile_pos < 0) {
        *tile_pos = tile_count + *tile_pos;
        --*tilemap_pos;
    }

    while (*tile_pos >= tile_count) {
        *tile_pos = *tile_pos - tile_count;
        ++*tilemap_pos;
    }
}

internal void
recanonicalize_coords(Tilemap *tilemap,
                      i32 tile_count,
                      s32 *tilemap_pos,
                      s32 *tile_pos,
                      r32 *tile_relative_pos) {
    s32 offset = floor_r32_to_s32(*tile_relative_pos / tilemap->tile_side_in_meters);

    *tile_pos += offset;

    *tile_relative_pos -= offset*tilemap->tile_side_in_meters;

    assert(*tile_relative_pos >= 0);
    assert(*tile_relative_pos < tilemap->tile_side_in_meters);

    if (*tile_pos < 0) {
        *tile_pos = tile_count + *tile_pos;
        --*tilemap_pos;
    }

    if (*tile_pos >= tile_count) {
        *tile_pos = *tile_pos - tile_count;
        ++*tilemap_pos;
    }
};

inline World_Position
recanonicalize_position(Tilemap *tilemap, World_Position position) {
    World_Position result = position;

    recanonicalize_coords(tilemap,
                          tilemap->count_x,
                          &result.tilemap_x,
                          &result.tile_x,
                          &result.tile_relative_x);
    recanonicalize_coords(tilemap,
                          tilemap->count_y,
                          &result.tilemap_y,
                          &result.tile_y,
                          &result.tile_relative_y);

    return result;
}

inline Room *
get_room(Tilemap *tilemap, i32 tilemap_x, i32 tilemap_y) {
    Room *room = 0;

    if (tilemap_x >= 0 && tilemap_x < tilemap->tilemap_count &&
        tilemap_y >= 0 && tilemap_y < tilemap->tilemap_count) {
        room = &tilemap->rooms[tilemap_y * tilemap->tilemap_count + tilemap_x];
    }

    assert(room);

    return room;
}

inline Room *
get_room(Tilemap *tilemap, s32 tilemap_x, s32 tilemap_y, s32 tile_x, s32 tile_y) {
    Room *room = 0;
    s32 og_tilemap_x = tilemap_x;
    s32 og_tilemap_y = tilemap_y;
    s32 og_tile_x = tile_x;
    s32 og_tile_y = tile_y;

    normalize_coords(tilemap->count_x, &tilemap_x, &tile_x);
    normalize_coords(tilemap->count_y, &tilemap_y, &tile_y);

    room = get_room(tilemap, tilemap_x, tilemap_y);

    normalize_coords(tilemap->count_x, &og_tilemap_x, &og_tile_x);
    normalize_coords(tilemap->count_y, &og_tilemap_y, &og_tile_y);

    if (og_tilemap_x != tilemap_x ||
        og_tilemap_y != tilemap_y ||
        og_tile_x != tile_x ||
        og_tile_y != tile_y) {
        room = get_room(tilemap, tilemap_x, tilemap_y);
    }

    assert(room);

    return room;
}



internal u32
get_tile_value(Tilemap *tilemap, Room *room, i32 tile_x, i32 tile_y) {
    assert(room);
    assert((tile_x >= 0) && (tile_x < tilemap->count_x) &&
           (tile_y >= 0) && (tile_y < tilemap->count_y));

    u32 tile_value = room->tiles[tile_y * tilemap->count_x + tile_x];

    return tile_value;
}

inline u32
get_tile_value(Tilemap *tilemap, s32 tilemap_x, s32 tilemap_y, s32 tile_x, s32 tile_y) {
    // NOTE: tile_value = 9 is considered an skippable tile. That means it will
    //       not be processed by the renderer thus saving miliseconds and frames.
    u32 tile_value = 9;

    normalize_coords(tilemap->count_x, &tilemap_x, &tile_x);
    normalize_coords(tilemap->count_y, &tilemap_y, &tile_y);

    Room *room = get_room(tilemap, tilemap_x, tilemap_y);
    assert(room);

    if (room && room->tiles) {
        tile_value = get_tile_value(tilemap, room, (u32)tile_x, (u32)tile_y);
    }

    return tile_value;
}

internal bool
is_room_point_empty(Tilemap *tilemap, Room *room, i32 test_tile_x, i32 test_tile_y) {
    bool is_empty = false;

    if (tilemap) {
        if (test_tile_x >= 0 &&
            test_tile_x < tilemap->count_x*tilemap->tile_side_in_pixels &&
            test_tile_y >= 0 &&
            test_tile_y < tilemap->count_y*tilemap->tile_side_in_pixels) {
            u32 tile_value = get_tile_value(tilemap, room, test_tile_x, test_tile_y);

            is_empty  = (tile_value == 0);
        }
    }

    return is_empty;

}

inline bool
is_world_point_empty(Tilemap *tilemap, World_Position world_position) {
    bool is_empty = false;

    Room *room = get_room(tilemap, world_position.tilemap_x, world_position.tilemap_y);
    is_empty = is_room_point_empty(tilemap,
                                   room,
                                   world_position.tile_x,
                                   world_position.tile_y);

    return is_empty;
}
