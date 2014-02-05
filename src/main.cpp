#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <memory>

#include <SDL2/SDL.h>

#include "main.h"
#include "errors.h"
#include "tgx.h"
#include "gm1.h"
#include "SDLContext.h"

int main()
{
    SDLContext sdl(SDL_INIT_EVERYTHING);
    SDL_Window *wnd;
    SDL_Renderer *renderer;
    if(0 != SDL_CreateWindowAndRenderer(640, 480, 0, &wnd, &renderer))
        throw SDLError("SDL_CreateWindowAndRenderer");
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

    std::vector<std::string> files = LoadStringList(std::string("gm1list.txt"));
    if(files.empty())
        throw "No files to load.";
    
    CollectionCursor cursor(renderer, files);
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
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                case SDLK_TAB:
                    cursor.SelectCollection(-1);
                    break;
                case SDLK_RETURN:
                    cursor.SelectCollection(1);
                    break;
                case SDLK_LEFT:
                    cursor.Collection()->SelectImage(-1);
                    break;
                case SDLK_RIGHT:
                    cursor.Collection()->SelectImage(1);
                    break;
                case SDLK_UP:
                    cursor.Collection()->SelectPalette(-1);
                    break;
                case SDLK_DOWN:
                    cursor.Collection()->SelectPalette(1);
                    break;
                }
                break;
            }
        }

        if(cursor.Collection()->Image() != NULL) {
            int width, height;
            SDL_GetWindowSize(wnd, &width, &height);
            // if((width != cursor.Collection()->Width())
            //    || (height != cursor.Collection()->Height()))
            //     SDL_SetWindowSize(wnd, cursor.Collection()->Width(), cursor.Collection()->Height());
            SDL_RenderCopy(renderer, cursor.Collection()->Image(), NULL, NULL);
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(wnd);
    
    return 0;
}

ImageCursor::ImageCursor(SDL_Renderer *render, std::shared_ptr<GM1Collection> arc)
    : arc(arc)
    , paletteIndex(0)
    , textureIndex(0)
    , render(render)
    , lastTexture(NULL)
    , invalidated(true)
{
}

ImageCursor::~ImageCursor()
{
    if(lastTexture != NULL)
        SDL_DestroyTexture(lastTexture);
}

int ImageCursor::Height()
{
    return arc->images[textureIndex].Height();
}

int ImageCursor::Width()
{
    return arc->images[textureIndex].Width();
}

void ImageCursor::SelectPalette(int shift)
{
    paletteIndex = (paletteIndex + GM1_PALETTE_COUNT + shift) % GM1_PALETTE_COUNT;
    invalidated = true;
}

void ImageCursor::SelectImage(int shift)
{
    textureIndex = (textureIndex + arc->ImageCount() + shift) % arc->ImageCount();
    invalidated = true;
}

SDL_Texture *ImageCursor::Image()
{
    if(invalidated) {
        if(lastTexture != NULL)
            SDL_DestroyTexture(lastTexture);
        lastTexture = BuildTextureFromCollection(render, arc, textureIndex, paletteIndex);
        SDL_Log("# %d [%d]", textureIndex, paletteIndex);
        DebugPrint_GM1ImageHeader(arc->images[textureIndex].header);
        invalidated = false;
    }
    return lastTexture;
}

CollectionCursor::CollectionCursor(SDL_Renderer *render, const std::vector<std::string> &list)
    : list(list)
    , currentIndex(0)
    , invalidated(true)
    , render(render)
    , cursor()
{
}

CollectionCursor::~CollectionCursor()
{
}

std::shared_ptr<ImageCursor> CollectionCursor::CreateImageCursor()
{
    std::string filename = list[currentIndex];
    return std::make_shared<ImageCursor>(render, LoadCollection(filename));
}

void CollectionCursor::SelectCollection(int shift)
{
    currentIndex = (currentIndex + shift + list.size()) % list.size();
    invalidated = true;
}

std::shared_ptr<ImageCursor> CollectionCursor::Collection()
{
    if(invalidated) {
        cursor = CreateImageCursor();        
        invalidated = false;
    }
    return cursor;
}

SDL_Texture* BuildTextureFromCollection(SDL_Renderer *renderer, const std::shared_ptr<GM1Collection> &arc, size_t textureIndex, size_t paletteIndex)
{
    switch(arc->images[textureIndex].encoding) {
    case ImageEncoding::TGX8:
        return
            arc->images[textureIndex].CreateAnimationTexture(
            renderer,
            arc->palettes[paletteIndex]);
    case ImageEncoding::TileObject:
        return arc->images[textureIndex].CreateTileTexture(renderer);
    case ImageEncoding::Bitmap:
    case ImageEncoding::TGX16:
    default:
        return arc->images[textureIndex].CreateBitmapTexture(
            renderer);
    }
}

std::shared_ptr<GM1Collection> LoadCollection(const std::string &filename)
{
    std::shared_ptr<GM1Collection> arc;
    SDL_RWops *src = SDL_RWFromFile(filename.c_str(), "rb");
    SDL_Log("GM1 file name: %s", filename.c_str());
    if(src == NULL)
        throw std::invalid_argument(filename);
    try {
        arc = std::make_shared<GM1Collection>(src);
    } catch(const EOFError &e) {
        SDL_Log("EOF reached: %s", e.what());
    } catch(const FormatError &e) {
        SDL_Log("Error happened while encoding: %s", e.what());
    }
    SDL_RWclose(src);
    return arc;
}

std::vector<std::string> LoadStringList(const std::string &filename)
{
    std::ifstream fin(filename);
    std::vector<std::string> list;
    while(!fin.eof()) {
        std::string s;
        std::getline(fin, s);
        if(!s.empty())
            list.push_back(s);
    }
    return list;
}
