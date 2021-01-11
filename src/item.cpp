#include "item.h"

#include "math.h"

void create_item(const char *name,
                 const char *texture_name,
                 Vector2 frame_position,
                 Vector2 frame_size,
                 Item_List *item_list) {
    Item new_item;

    new_item.name = name;
    new_item.texture_name = texture_name;
    new_item.size = frame_size;
    new_item.frame_position = frame_position;
    new_item.frame_size = frame_size;

    item_list->il[item_list->count] = new_item;
    item_list->count += 1;
}
