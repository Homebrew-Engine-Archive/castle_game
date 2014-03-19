#include "textrenderer.h"
#include <iostream>
#include "geometry.h"
#include "atlas.h"

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

struct TextRendererPimpl
{
    SDL_Renderer *renderer;
    const FontData *fontData;
    SDL_Color color;
    int cursorX;
    int cursorY;
    std::vector<FontData> fonts;

    TextRendererPimpl(SDL_Renderer *renderer_);
    
    TexturePtr CreateFontAtlas(const Font &font, std::vector<SDL_Rect> &partition) const;
    SDL_Rect FindTextureSubrect(int character) const;    
    const GlyphData *FindGlyphData(int character) const;

    void PutChar(int character);
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

TextRendererPimpl::TextRendererPimpl(SDL_Renderer *renderer_)
    : renderer {renderer_}
    , color {0, 0, 0, 0}
    , cursorX {0}
    , cursorY {0}
{ }

void TextRendererPimpl::PutChar(int character)
{
    if(fontData != NULL) {
        SDL_Texture *texture = fontData->texture.get();
        SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(texture, color.a);
        
        const GlyphData *glyphData = FindGlyphData(character);
        if(glyphData != NULL) {
            SDL_Rect srcRect = FindTextureSubrect(character);
            SDL_Rect dstRect = GetGlyphFaceBox(*glyphData, cursorX, cursorY);
            ThrowSDLError(
                SDL_RenderCopy(renderer, texture, &srcRect, &dstRect));
            SDL_Rect advanceBox = GetGlyphAdvanceBox(*glyphData, cursorX, cursorY);
            cursorX += advanceBox.w;
        }
    }
}
    

const GlyphData *TextRendererPimpl::FindGlyphData(int character) const
{
    if(fontData != NULL) {
        return fontData->font.FindGlyph(character);
    }
    return NULL;
}

SDL_Rect TextRendererPimpl::FindTextureSubrect(int character) const
{
    if(fontData != NULL) {
        size_t idx = fontData->font.GetGlyphIndex(character);
        if(idx < fontData->partition.size())
            return fontData->partition[idx];
    }
    return MakeEmptyRect();
}

TexturePtr TextRendererPimpl::CreateFontAtlas(const Font &font, std::vector<SDL_Rect> &partition) const
{
    std::vector<Surface> surfaces;
    for(const GlyphData &glyph : font.GetGlyphList()) {
        surfaces.push_back(glyph.face);
    }

    SDL_RendererInfo info;
    if(SDL_GetRendererInfo(renderer, &info) < 0) {
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
        SDL_CreateTextureFromSurface(renderer, atlas));
    if(!ptr) {
        std::cerr << "SDL_CreateTextureFromSurface failed: "
                  << SDL_GetError()
                  << std::endl;
        return TexturePtr(nullptr);
    }
    
    return ptr;
}

TextRenderer::TextRenderer(SDL_Renderer *renderer)
    : m {new TextRendererPimpl(renderer)}
{ }

TextRenderer::~TextRenderer()
{
    delete m;
}

void TextRenderer::PutString(const std::string &str)
{
    for(int character : str) {
        m->PutChar(character);
    }
}

SDL_Rect TextRenderer::CalculateTextRect(const std::string &str) const
{
    SDL_Rect bounds = MakeEmptyRect();
    if(m->fontData != NULL) {
        int x = m->cursorX;
        for(int character : str) {
            const GlyphData *glyphData = m->FindGlyphData(character);
            if(glyphData != NULL) {
                SDL_Rect glyphRect = GetGlyphAdvanceBox(*glyphData, x, m->cursorY);
                SDL_UnionRect(&glyphRect, &bounds, &bounds);
                x += glyphRect.w;
            }
        }
    }
    return bounds;
}

bool TextRenderer::CacheFont(const std::string &name, int size, const Font &font)
{
    FontData fontData;
    fontData.fontname = name;
    fontData.size = size;
    fontData.font = font;

    fontData.texture =
        std::move(
            m->CreateFontAtlas(font, fontData.partition));
    if(!fontData.texture) {
        std::cerr << "Failed to create font atlas" << std::endl;
        return false;
    }

    m->fonts.push_back(std::move(fontData));
    return true;
}

void TextRenderer::SetCursor(const SDL_Point &cursor)
{
    m->cursorX = cursor.x;
    m->cursorY = cursor.y;
}

void TextRenderer::SetColor(const SDL_Color &color)
{
    m->color = color;
}

bool TextRenderer::SetFont(const std::string &fontname, int size)
{
    if(m->fontData != NULL) {
        if((m->fontData->fontname == fontname) && (m->fontData->size == size))
            return true;
    }
    m->fontData = NULL;
    for(const auto &font : m->fonts) {
        m->fontData = GetBestMatch(fontname, size, m->fontData, &font);
    }
    if(m->fontData == NULL) {
        return false;
    }
    return true;
}

bool TextRenderer::SetFontSize(int size)
{
    std::string fontName;
    if(m->fontData != NULL) {
        fontName = m->fontData->fontname;
    }
    return SetFont(fontName, size);
}

bool TextRenderer::SetFontName(const std::string &name)
{
    int size = 0;    
    if(m->fontData != NULL) {
        size = m->fontData->size;
    }
    return SetFont(name, size);
}
