#include "font.h"

GM1Font::GM1Font(const CollectionAtlas &atlas, const std::vector<int> &alphabet, size_t skip, size_t count, int yoffset)
{
    
}

bool GM1Font::PutGlyph(Renderer *renderer, int character, const SDL_Point &point, const SDL_Color &color) const
{
    if(renderer == NULL) {
        std::cerr << "PutGlyph: passed NULL renderer" << std::endl;
        return false;
    }

    return true;
}

SDL_Rect GM1Font::GetGlyphBounds(int character) const
{
}
