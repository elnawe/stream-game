#include "world.h"

#include "game.h"
#include "SDL.h"
#include "texture_manager.h"

static const int8_t size_X = 22;
static const int8_t size_Y = 11;
static intptr_t tilemap[size_Y][size_X] = {
    {4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 7},
};

void create_tilemap() {

}

void render_tilemap(Game_Data *game) {
    SDL_Texture *texture = get_texture("TILESET", &game->texture_manager);
    int tile_size = 32;
    for (int X = 0; X < size_X; ++X) {
        for (int Y = 0; Y < size_Y; ++Y) {
            SDL_Rect src_rect, dst_rect;
            src_rect.x = tilemap[Y][X] * tile_size;
            src_rect.y = 0;
            src_rect.w = tile_size;
            src_rect.h = tile_size;

            dst_rect.x = (X * tile_size);
            dst_rect.y = Y * tile_size;
            dst_rect.w = tile_size;
            dst_rect.h = tile_size;

            SDL_RenderCopy(game->renderer,
                           texture,
                           &src_rect,
                           &dst_rect);
        }
    }
}
