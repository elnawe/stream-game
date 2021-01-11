#ifndef ITEM_H
#define ITEM_H

#include "math.h"

struct Item {
    const char *name;
    const char *texture_name;
    Vector2 size;

    Vector2 frame_position;
    Vector2 frame_size;
};

struct Item_List {
    Item il[20];
    int count;
};

void create_item(const char *name,
                 const char *texture_name,
                 Vector2 frame_position,
                 Vector2 frame_size,
                 Item_List *item_list);

#endif
