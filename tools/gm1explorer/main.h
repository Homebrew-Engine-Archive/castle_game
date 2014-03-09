#ifndef MAIN_H_
#define MAIN_H_

#include <string>
#include <map>
#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <exception>
#include <cassert>

#include "atlas.h"
#include "sdl_utils.h"
#include "surface.h"
#include "tgx.h"
#include "gm1.h"
#include "rw.h"

int explorer_main(const std::string &filename);

class TextureLocker
{
    SDL_Texture *texture;
    bool locked;
public:
    TextureLocker(SDL_Texture *t, const SDL_Rect *rect, void **pixels, int *pitch)
        : texture(t)
        , locked(false) {
        if(SDL_LockTexture(texture, rect, pixels, pitch) < 0)
            throw std::runtime_error(SDL_GetError());
        locked = true;
    }
    ~TextureLocker() {
        if(locked)
            SDL_UnlockTexture(texture);
    }
};

class viewer_t
{
    size_t paletteIndex;
    size_t imageIndex;
    RendererPtr renderer;
    WindowPtr window;
    SDL_Rect selected;
    int offsetx;
    int offsety;
    Surface atlas;
    std::vector<SDL_Rect> partitions;
    std::vector<Surface> surfaces;
    FileBuffer data;
    gm1::Collection collection;
    std::vector<PalettePtr> palettes;
    bool quit;
    int frameLimit;
    int frameRate;
    int screenWidth;
    int screenHeight;
    TexturePtr streamTexture;
    bool showGreenRect;
    bool showRedRect;
    bool showAllRects;
    bool showSingleImage;
    
public:
    viewer_t(const gm1::Collection &collection, const FileBuffer &data);
    void print_renderer_info();
    void alloc_window();
    void alloc_renderer();
    void alloc_texture(int width, int height);
    void alloc_palettes();
    void enter_event_loop();
    void update_texture(SDL_Texture *texture);
    void update_surface(Surface screen);
    SDL_Palette *get_palette_by_index(size_t index);
    size_t cycle_ref(size_t index, size_t max, int d);
    void set_cursor(int n, int m);
    void handle_key(SDL_Keycode code);
    void handle_mouse_down(int x, int y);
    void draw();
    void draw_entry(Surface dst, int x, int y, const gm1::Collection &gm1, size_t index);
    
};

#endif
