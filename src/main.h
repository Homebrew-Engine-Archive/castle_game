#ifndef MAIN_H_
#define MAIN_H_

#include <functional>
#include <vector>
#include <memory>
#include <string>
#include <SDL2/SDL.h>

#include "errors.h"
#include "gm1.h"
#include "tgx.h"

std::vector<std::string> LoadStringList(const std::string &filename);
std::shared_ptr<GM1Collection> LoadCollection(const std::string &filename);
SDL_Texture* BuildTextureFromCollection(SDL_Renderer *renderer, const std::shared_ptr<GM1Collection> &arc, size_t imageIndex, size_t paletteIndex);

class ImageCursor
{
    std::shared_ptr<GM1Collection> arc;
    size_t paletteIndex;
    size_t textureIndex;
    SDL_Renderer *render;
    SDL_Texture *lastTexture;
    bool invalidated;
    
public:
    ImageCursor(SDL_Renderer *render, std::shared_ptr<GM1Collection> arc);
    ~ImageCursor();
    void SelectPalette(int shift);
    void SelectImage(int shift);
    int Width();
    int Height();
    SDL_Texture *Image();
};

class CollectionCursor
{
    std::vector<std::string> list;
    size_t currentIndex;
    bool invalidated;
    SDL_Renderer *render;
    std::shared_ptr<ImageCursor> cursor;
    
    std::shared_ptr<ImageCursor> CreateImageCursor();
    
public:
    CollectionCursor(SDL_Renderer *render, const std::vector<std::string> &list);
    ~CollectionCursor();
    std::shared_ptr<ImageCursor> Collection();
    void SelectCollection(int shift);
};

#endif
