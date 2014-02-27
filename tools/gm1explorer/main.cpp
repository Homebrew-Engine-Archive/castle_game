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

using namespace std;

class viewer_t
{
    size_t paletteIndex;
    size_t imageIndex;
    SDL_Renderer *renderer;
    SDL_Window *window;
    gm1::Collection collection;
    Surface atlas;
    bool quit;
    std::vector<SDL_Rect> rects;
    std::map<size_t, SDL_Palette *> palettes;
    
public:
    viewer_t(const gm1::Collection &collection, const Surface &surface, const std::vector<SDL_Rect> &rects)
        : paletteIndex(0)
        , imageIndex(0)
        , collection(collection)
        , atlas(surface)
        , quit(false)
        , rects(rects) {
        if(SDL_CreateWindowAndRenderer(640, 480, SDL_WINDOW_RESIZABLE, &window, &renderer))
            throw std::runtime_error(SDL_GetError());
        PickImageAndPalette(0, 0);
    }
    
    ~viewer_t() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        for(const auto &p : palettes) {
            SDL_FreePalette(p.second);
        }
    }
    
    void enter_event_loop() {
        quit = false;
        SDL_Event e;
        while(!quit) {
            while(SDL_PollEvent(&e)) {
                switch(e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    handle_key(e.key.keysym.sym);
                    break;
                }
            }
            const SDL_Rect &current = rects[imageIndex];
            SDL_Palette *palette = get_palette_by_index(paletteIndex);
            draw_image(current, palette);
            SDL_Delay(40);
        }
    }

    SDL_Palette *get_palette_by_index(size_t index) {
        auto searchResult = palettes.find(index);
        if(searchResult != palettes.end()) {
            return searchResult->second;
        } else {
            SDL_Log("Allocate new palette with index %d", index);
            SDL_Palette *palette =
                gm1::CreateSDLPaletteFrom(collection.palettes[index]);
            palettes.insert(std::make_pair(index, palette));
            return palette;
        }
    }
    
    size_t SetIndex(size_t index, size_t max, int d) {
        if(max == 0)
            throw std::invalid_argument("max in SetIndex");
        return ((int)index + (int)max + d) % max;
    }

    void PickImageAndPalette(int n, int m) {
        imageIndex = SetIndex(
            imageIndex, gm1::GetImageCount(collection.header), n);
        paletteIndex = SetIndex(
            paletteIndex, GM1_PALETTE_COUNT, m);
        gm1::VerbosePrintImageHeader(collection.headers[imageIndex]);
    }
    
    void handle_key(SDL_Keycode code) {
        switch(code) {
        case SDLK_ESCAPE:
            quit = true;
            break;
        case SDLK_RIGHT:
            PickImageAndPalette(1, 0);
            break;
        case SDLK_LEFT:
            PickImageAndPalette(-1, 0);
            break;
        case SDLK_UP:
            PickImageAndPalette(0, 1);
            break;
        case SDLK_DOWN:
            PickImageAndPalette(0, -1);
            break;
        case SDLK_RETURN:
            resize_window(rects[imageIndex]);
            break;
        }
    }
    
    void draw_image(const SDL_Rect &current, SDL_Palette *palette) {
        Surface dst = CopySurface(atlas, &current);
        if(dst->format->BitsPerPixel == 8)
            if(SDL_SetSurfacePalette(dst, palette))
                throw std::runtime_error(SDL_GetError());

        SDL_Texture *texture = SDL_CreateTextureFromSurface(
            renderer, dst);

        if(texture == NULL)
            throw std::runtime_error(SDL_GetError());

        if(SDL_RenderClear(renderer))
            throw std::runtime_error(SDL_GetError());
        
        if(SDL_RenderCopy(renderer, texture, NULL, NULL))
            throw std::runtime_error(SDL_GetError());
        
        SDL_RenderPresent(renderer);

        SDL_UnlockTexture(texture);
        SDL_DestroyTexture(texture);
    }
    
    void resize_window(const SDL_Rect &rect) {
        int w;
        int h;
        SDL_GetWindowSize(window, &w, &h);
        if(w != rect.w || h != rect.h) {
            SDL_SetWindowSize(window, rect.w, rect.h);
        }
    }
};

int explorer_main(const std::string &filename)
{
    try {
        SDL_RWops *src = SDL_RWFromFile(filename.c_str(), "rb");
        if(src == NULL)
            throw std::runtime_error(SDL_GetError());

        gm1::Collection collection(src);
        Surface atlas = gm1::LoadAtlas(src, collection);
        SDL_RWclose(src);

        gm1::VerbosePrintHeader(collection.header);

        std::vector<SDL_Rect> partition;
        gm1::PartitionAtlas(collection, partition);        
        
        viewer_t viewer(collection, atlas, partition);
        viewer.enter_event_loop();
        return 0;
        
    } catch(const std::exception &e) {
        SDL_Log("Exception: %s", e.what());
        return -1;
    }    
}

int main(int argc, const char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    if(argc > 1) {
        std::string filename = argv[1];
        return explorer_main(filename);
    }
    SDL_Quit();
    return 0;
}
