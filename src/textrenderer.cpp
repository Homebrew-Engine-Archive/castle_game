#include "textrenderer.h"
#include "geometry.h"
#include "text.h"
#include "atlas.h"
#include "SDL.h"
#include "font.h"
#include "macrosota.h"
#include <iostream>
#include <vector>

namespace
{

    /**
     * This structure represent font in context of a renderer, so
     * it can't be copied to another renderer.
     * 
     */
    struct FontData
    {
        std::string fontname;
        int size;
        Font font;
        TexturePtr texture;
        std::vector<SDL_Rect> partition;
        FontData() = default;
        FontData(const FontData &) = delete;
        FontData(FontData &&) = default;
        FontData &operator=(const FontData &) = delete;
        FontData &operator=(FontData &&) = default;
    };

    /**
     * Matches two fonts select the best one for given fontname and size.
     */
    const FontData *GetBestMatch(const std::string &fn, int size, const FontData *lhs, const FontData *rhs)
    {
        if(lhs == NULL)
            return rhs;
        if(rhs == NULL)
            return lhs;
        if((lhs->fontname == fn) && (rhs->fontname != fn))
            return lhs;
        if((lhs->fontname != fn) && (rhs->fontname == fn))
            return rhs;
        // Best size is the largest one but not greater than `size'
        // or else the smallest one
        if(lhs->size > rhs->size) {
            if(lhs->size > size) {
                return rhs;
            } else {
                return lhs;
            }
        } else {
            if(rhs->size > size) {
                return rhs;
            } else {
                return lhs;
            }
        }
    }

    SDL_Rect GetGlyphFaceBox(const GlyphData &glyph, int x, int y)
    {
        return MakeRect(
            x + glyph.xbearing,
            y - glyph.ybearing,
            glyph.hbox,
            glyph.vbox);
    }

    SDL_Rect GetGlyphAdvanceBox(const GlyphData &glyph, int x, int y)
    {
        return MakeRect(
            x + glyph.xbearing,
            y - glyph.ybearing,
            glyph.xadvance,
            glyph.vbox);
    }

}

class TextRendererImpl final : public TextRenderer
{
    SDL_Renderer *mRenderer;
    SDL_Color mColor;
    int mCursorX;
    int mCursorY;
    std::vector<FontData> mFonts;
    const FontData *mFontData;

    TexturePtr CreateFontAtlas(const Font &font, std::vector<SDL_Rect> &partition) const;
    SDL_Rect FindTextureSubrect(int character) const;    
    const GlyphData *FindGlyphData(int character) const;
    void PutChar(int character);

public:
    TextRendererImpl(SDL_Renderer *mRenderer);
    TextRendererImpl(const TextRendererImpl &) = delete;
    TextRendererImpl(TextRendererImpl &&) = default;
    TextRendererImpl &operator=(const TextRendererImpl &) = delete;
    TextRendererImpl &operator=(TextRendererImpl &&) = default;
    
    bool CacheFont(const std::string &name, int size, const Font &font);
    bool SetFont(const std::string &name, int size);
    bool SetFontName(const std::string &name);
    bool SetFontSize(int size);
    void SetColor(const SDL_Color &mColor);
    void SetCursor(const SDL_Point &cursor);
    SDL_Rect CalculateTextRect(const std::string &str) const;
    void PutString(const std::string &str);
};

TextRendererImpl::TextRendererImpl(SDL_Renderer *renderer)
    : mRenderer(renderer)
    , mColor {0, 0, 0, 0}
    , mCursorX(0)
    , mCursorY(0)
    , mFontData(NULL)
{ }

void TextRendererImpl::PutChar(int character)
{
    if(mFontData != NULL) {
        SDL_Texture *texture = mFontData->texture.get();
        SDL_SetTextureColorMod(texture, mColor.r, mColor.g, mColor.b);
        SDL_SetTextureAlphaMod(texture, mColor.a);
        
        const GlyphData *glyphData = FindGlyphData(character);
        if(glyphData != NULL) {
            SDL_Rect srcRect = FindTextureSubrect(character);
            SDL_Rect dstRect = GetGlyphFaceBox(*glyphData, mCursorX, mCursorY);
            ThrowSDLError(
                SDL_RenderCopy(mRenderer, texture, &srcRect, &dstRect));
            SDL_Rect advanceBox = GetGlyphAdvanceBox(*glyphData, mCursorX, mCursorY);
            mCursorX += advanceBox.w;
        }
    }
}
    

const GlyphData *TextRendererImpl::FindGlyphData(int character) const
{
    if(mFontData != NULL) {
        return mFontData->font.FindGlyph(character);
    }
    return NULL;
}

SDL_Rect TextRendererImpl::FindTextureSubrect(int character) const
{
    if(mFontData != NULL) {
        size_t idx = mFontData->font.GetGlyphIndex(character);
        if(idx < mFontData->partition.size())
            return mFontData->partition[idx];
    }
    return MakeEmptyRect();
}

TexturePtr TextRendererImpl::CreateFontAtlas(const Font &font, std::vector<SDL_Rect> &partition) const
{
    std::vector<Surface> surfaces;
    for(const GlyphData &glyph : font.GetGlyphList()) {
        surfaces.push_back(glyph.face);
    }

    SDL_RendererInfo info;
    if(SDL_GetRendererInfo(mRenderer, &info) < 0) {
        std::cerr << "Failed to get driver info" << std::endl;
        return TexturePtr(nullptr);
    }
    int maxWidth = info.max_texture_width;
    int maxHeight = info.max_texture_height;
    
    Surface atlas = MakeSurfaceAtlas(
        surfaces, partition, maxWidth, maxHeight);
    if(atlas.Null()) {
        std::cerr << "Failed to make atlas of surfaces" << std::endl;
        return TexturePtr(nullptr);
    }
    
    TexturePtr ptr = TexturePtr(
        SDL_CreateTextureFromSurface(mRenderer, atlas));
    if(!ptr) {
        std::cerr << "SDL_CreateTextureFromSurface failed: "
                  << SDL_GetError()
                  << std::endl;
        return TexturePtr(nullptr);
    }
    
    return ptr;
}

void TextRendererImpl::PutString(const std::string &str)
{
    for(int character : str) {
        PutChar(character);
    }
}

SDL_Rect TextRendererImpl::CalculateTextRect(const std::string &str) const
{
    SDL_Rect bounds = MakeEmptyRect();
    if(mFontData != NULL) {
        int x = mCursorX;
        for(int character : str) {
            const GlyphData *glyphData = FindGlyphData(character);
            if(glyphData != NULL) {
                SDL_Rect glyphRect = GetGlyphAdvanceBox(*glyphData, x, mCursorY);
                SDL_UnionRect(&glyphRect, &bounds, &bounds);
                x += glyphRect.w;
            }
        }
    }
    return bounds;
}

bool TextRendererImpl::CacheFont(const std::string &name, int size, const Font &font)
{
    FontData fontData;
    fontData.fontname = name;
    fontData.size = size;
    fontData.font = font;

    fontData.texture =
        std::move(
            CreateFontAtlas(font, fontData.partition));
    if(!fontData.texture) {
        std::cerr << "Failed to create font atlas" << std::endl;
        return false;
    }

    mFonts.push_back(std::move(fontData));
    
    return true;
}

void TextRendererImpl::SetCursor(const SDL_Point &cursor)
{
    mCursorX = cursor.x;
    mCursorY = cursor.y;
}

void TextRendererImpl::SetColor(const SDL_Color &color)
{
    mColor = color;
}

bool TextRendererImpl::SetFont(const std::string &fontname, int size)
{
    if(mFontData != NULL) {
        if((mFontData->fontname == fontname) && (mFontData->size == size))
            return true;
    }
    for(const FontData &font : mFonts) {
        mFontData = GetBestMatch(fontname, size, mFontData, &font);
    }
    if(mFontData == NULL) {
        return false;
    }
    return true;
}

bool TextRendererImpl::SetFontSize(int size)
{
    std::string fontName;
    if(mFontData != NULL) {
        fontName = mFontData->fontname;
    }
    return SetFont(fontName, size);
}

bool TextRendererImpl::SetFontName(const std::string &name)
{
    int size = 0;    
    if(mFontData != NULL) {
        size = mFontData->size;
    }
    return SetFont(name, size);
}

std::unique_ptr<TextRenderer> CreateTextRenderer(SDL_Renderer *renderer)
{
    return make_unique<TextRendererImpl>(renderer);
}
