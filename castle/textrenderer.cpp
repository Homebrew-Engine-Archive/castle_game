#include "textrenderer.h"

#include <sstream>
#include <iostream>
#include <vector>
#include <SDL.h>

#include "geometry.h"
#include "text.h"
#include "atlas.h"
#include "font.h"
#include "macrosota.h"

namespace
{
    
    /**
     * This structure represent font in context of a renderer, so
     * it can't be copied to another renderer.
     * 
     */
    struct FontData
    {
        std::string mName;
        int mSize;
        Render::Font mFont;
        TexturePtr mTexture;
        std::vector<SDL_Rect> mPartition;
        FontData() = default;
        FontData(const FontData &) = delete;
        FontData(FontData &&) = default;
        FontData &operator=(const FontData &) = delete;
        FontData &operator=(FontData &&) = default;
        ~FontData() = default;
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
        if((lhs->mName == fn) && (rhs->mName != fn))
            return lhs;
        if((lhs->mName != fn) && (rhs->mName == fn))
            return rhs;
        // Best size is the largest one but not greater than `size'
        // or else the smallest one
        if(lhs->mSize > rhs->mSize) {
            if(lhs->mSize > size) {
                return rhs;
            } else {
                return lhs;
            }
        } else {
            if(rhs->mSize > size) {
                return rhs;
            } else {
                return lhs;
            }
        }
    }

    SDL_Rect GetGlyphFaceBox(const Render::GlyphData &glyph, int x, int y)
    {
        return MakeRect(
            x + glyph.xbearing,
            y - glyph.ybearing,
            glyph.hbox,
            glyph.vbox);
    }

    SDL_Rect GetGlyphAdvanceBox(const Render::GlyphData &glyph, int x, int y)
    {
        return MakeRect(
            x + glyph.xbearing,
            y - glyph.ybearing,
            glyph.xadvance,
            glyph.vbox);
    }

    void Fail(const std::string &where, const std::string &what)
    {
        std::ostringstream oss;
        oss << where << " failed: " << what;
        throw std::runtime_error(oss.str());
    }
    
}

namespace Render
{

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
        , mFonts()
        , mFontData(nullptr)
        { }

    void TextRendererImpl::PutChar(int character)
    {
        if(mFontData != nullptr) {
            SDL_Texture *tex = mFontData->mTexture.get();
            
            if(SDL_SetTextureColorMod(tex, mColor.r, mColor.g, mColor.b) < 0) {
                Fail("PutChar", SDL_GetError());
            }

            if(SDL_SetTextureAlphaMod(tex, mColor.a) < 0) {
                Fail("PutChar", SDL_GetError());
            }
        
            const GlyphData *glyphData = FindGlyphData(character);
            if(glyphData != nullptr) {
                SDL_Rect srcRect = FindTextureSubrect(character);
                SDL_Rect dstRect = GetGlyphFaceBox(*glyphData, mCursorX, mCursorY);
                ThrowSDLError(
                    SDL_RenderCopy(mRenderer, tex, &srcRect, &dstRect));
                SDL_Rect advanceBox = GetGlyphAdvanceBox(*glyphData, mCursorX, mCursorY);
                mCursorX += advanceBox.w;
            }
        }
    }
    

    const GlyphData *TextRendererImpl::FindGlyphData(int character) const
    {
        if(mFontData != nullptr) {
            return mFontData->mFont.FindGlyph(character);
        }
        return nullptr;
    }

    SDL_Rect TextRendererImpl::FindTextureSubrect(int character) const
    {
        if(mFontData != nullptr) {
            size_t idx = mFontData->mFont.GetGlyphIndex(character);
            if(idx < mFontData->mPartition.size())
                return mFontData->mPartition[idx];
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
            Fail("CreateFontAtlas", SDL_GetError());
        }
        
        int maxWidth = info.max_texture_width;
        int maxHeight = info.max_texture_height;
        Surface atlas = MakeSurfaceAtlas(surfaces, partition, maxWidth, maxHeight);
        
        if(atlas.Null()) {
            Fail("CreateFontAtlas", "Failed to make surface atlas");
        }
    
        TexturePtr ptr(SDL_CreateTextureFromSurface(mRenderer, atlas));
        if(!ptr) {
            Fail("CreateFontAtlas", SDL_GetError());
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
        FontData newFontData;
        newFontData.mName = name;
        newFontData.mSize = size;
        newFontData.mFont = font;

        newFontData.mTexture =
            std::move(
                CreateFontAtlas(font, newFontData.mPartition));
        if(!newFontData.mTexture) {
            std::cerr << "Failed to create font atlas" << std::endl;
            return false;
        }

        mFonts.push_back(std::move(newFontData));
    
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
        if(mFontData != nullptr) {
            if((mFontData->mName == fontname) && (mFontData->mSize == size))
                return true;
        }
        
        for(const FontData &font : mFonts) {
            mFontData = GetBestMatch(fontname, size, mFontData, &font);
        }
        
        return (mFontData != nullptr);
    }

    bool TextRendererImpl::SetFontSize(int size)
    {
        std::string fontName;
        
        if(mFontData != nullptr) {
            fontName = mFontData->mName;
        }
        
        return SetFont(fontName, size);
    }

    bool TextRendererImpl::SetFontName(const std::string &name)
    {
        int size = 0;
        
        if(mFontData != nullptr) {
            size = mFontData->mSize;
        }
        
        return SetFont(name, size);
    }

    std::unique_ptr<TextRenderer> CreateTextRenderer(SDL_Renderer *renderer)
    {
        return std::make_unique<TextRendererImpl>(renderer);
    }

} // namespace Render
