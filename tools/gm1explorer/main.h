#ifndef MAIN_H_
#define MAIN_H_

#include <string>
#include <map>
#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <exception>
#include <cassert>

#include "surface.h"
#include "tgx.h"
#include "gm1.h"
#include "rw.h"

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
    
    SDL_Renderer *renderer;
    SDL_Window *window;

    Surface atlas;
    
    FileBuffer data;
    gm1::Collection collection;

    std::vector<SDL_Palette *> palettes;

    bool quit;
    
    int frameLimit;
    int frameRate;    
    
    int screenWidth;
    int screenHeight;
    
    SDL_Texture *streamTexture;
    
public:
    viewer_t(const gm1::Collection &collection, const FileBuffer &data);
    ~viewer_t();
    void print_renderer_info();
    void alloc_window();
    void dealloc_window();
    void alloc_renderer();
    void dealloc_renderer();
    void alloc_texture();
    void dealloc_texture();
    void alloc_palettes();
    void dealloc_palettes();
    void enter_event_loop();
    void update_texture(SDL_Texture *texture);
    void update_surface(Surface screen);
    SDL_Palette *get_palette_by_index(size_t index);
    size_t cycle_ref(size_t index, size_t max, int d);
    void set_cursor(int n, int m);
    void handle_key(SDL_Keycode code);    
    void draw();
    void draw_entry(Surface dst, int x, int y, const gm1::Collection &gm1, size_t index);
    
};

#endif
