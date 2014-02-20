#ifndef TEXTURE_H_
#define TEXTURE_H_

struct BaseTexture
{
    virtual void Blit(SDL_Renderer *renderer, const SDL_Rect *srcrect, SDL_Rect *dstrect) = 0;
};

class Texture : public BaseTexture
{
    SDL_Texture *texture;
    std::vector<SDL_Rect> rects;
    std::vector<gm1::ImageHeaders> headers;
    gm1::Header header;
    const SDL_Palette *palettes;
    size_t palettesCount;
    
public:
    
};

#endif
