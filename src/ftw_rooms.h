#ifndef FTW_ROOMS_H
#define FTW_ROOMS_H

// TODO: Move to using the enumaration in the future. Maybe?
enum RoomDoorExit {
    RoomDoorExit_N,
    RoomDoorExit_E,
    RoomDoorExit_S,
    RoomDoorExit_W,

    RoomDoorExit_NE,
    RoomDoorExit_NS,
    RoomDoorExit_NW,

    RoomDoorExit_ES,
    RoomDoorExit_EW,

    RoomDoorExit_NESW
};

struct Room_Exit {
    bool north;
    bool east;
    bool south;
    bool west;
};

struct Room {
    Room_Exit exit;

    u32 *tiles;
};

#endif
