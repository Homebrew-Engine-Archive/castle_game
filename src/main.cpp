#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include <SDL2/SDL.h>

#include "TGX.h"
#include "SDLContext.h"

SDL_Texture * LoadTexture(SDL_Renderer *render, const std::string &filename, Uint16 *width = NULL, Uint16 *height = NULL)
{
    SDL_RWops *src = SDL_RWFromFile(filename.c_str(), "rb");
    
    TGXHeader header;
    ReadTGXHeader(src, &header);

    if(width != NULL)
        *width = header.width;

    if(height != NULL)
        *height = header.height;
    
    size_t sz = header.width * header.height;
    Uint16 *pixels = new Uint16[header.width * header.height];
    for(size_t n = 0; n < sz; ++n)
        pixels[n] = 0;
    ReadTGXPixelsNoPalette(src, header.width, header.height, pixels);

    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(
        pixels, header.width, header.height, 16, header.width * 2,
        0x7c00, 0x3e0, 0x1f, 0);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(render, surface);
    SDL_FreeSurface(surface);
    delete [] pixels;
    src->close(src);

    return texture;
}

std::string GetRandomTextureName(const std::vector<std::string> &filenames)
{
    srand(time(NULL));
    return filenames.at(rand() % filenames.size());
}

std::vector<std::string> LoadTextureList(const std::string &filename)
{
    std::ifstream fin(filename);
    std::vector<std::string> list;
    while(!fin.eof()) {
        std::string s;
        std::getline(fin, s);
        list.push_back(s);
    }
    return list;
}

int main(int argc, const char *argv[])
{
    SDLContext context(SDL_INIT_EVERYTHING);

    SDL_Window *wnd;
    SDL_Renderer *render;
    SDL_CreateWindowAndRenderer(640, 480, 0, &wnd, &render);

    std::vector<std::string> textures = LoadTextureList(std::string("textures.txt"));

    Uint16 width, height;
    size_t currIndex = 0;
    SDL_Texture *texture = LoadTexture(render, textures[currIndex].c_str(), &width, &height);
    SDL_SetWindowSize(wnd, width, height);
    bool quit = false;
    while(!quit) {
        SDL_Event e;
        if(SDL_PollEvent(&e)) {
            switch(e.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                switch(e.key.keysym.sym) {
                case SDLK_LEFT:
                    if(currIndex != 0) {
                        currIndex -= 1;
                        texture = LoadTexture(render, textures[currIndex].c_str(), &width, &height);
                        SDL_SetWindowSize(wnd, width, height);
                    }
                    break;
                case SDLK_RIGHT:
                    if(currIndex != textures.size() - 1) {
                        currIndex += 1;
                        texture = LoadTexture(render, textures[currIndex].c_str(), &width, &height);
                        SDL_SetWindowSize(wnd, width, height);
                    }
                    break;
                }
                break;
            }
        }
        SDL_RenderCopy(render, texture, NULL, NULL);
        SDL_RenderPresent(render);
        SDL_Delay(10);
    }
    
    SDL_DestroyWindow(wnd);
    SDL_DestroyRenderer(render);
    SDL_DestroyTexture(texture);
    
    return 0;
}
