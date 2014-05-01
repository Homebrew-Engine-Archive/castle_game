#ifndef TEXTRENDERER_H_
#define TEXTRENDERER_H_

#include <memory>
#include <string>

#include <SDL.h>
#include <SDL_ttf.h>

#include <game/sdl_utils.h>
#include <game/atlas.h>

namespace Render
{
    enum class CursorMode : int {
        TopLeft,
        BaseLine,
        BottomLeft
    };
    
    class TextRenderer
    {
        Surface &mSurface;
        SDL_Color mColor {0, 0, 0, 0};
        int mCursorX = 0;
        int mCursorY = 0;
        CursorMode mCursorMode = CursorMode::BaseLine;
        TTF_Font *mCurrentFont = nullptr;

        SDL_Point GetTopLeftBoxPoint() const;
        void PutRenderedString(Surface &text);

    public:
        TextRenderer(Surface &surface);
        TextRenderer(TextRenderer const&) = delete;
        TextRenderer& operator=(TextRenderer const&) = delete;

        virtual void PutChar(int character);
        virtual void PutString(const std::string &str);
        virtual void PutString(const std::u16string &str);

        
        void SetFont(TTF_Font *font);
        TTF_Font* GetFont();
        void SetColor(const SDL_Color &color);
        void SetCursorMode(CursorMode mode);
        void LoadIdentity();
        void Translate(int dx, int dy);
        
        SDL_Rect CalculateTextRect(const std::u16string &str) const;
        SDL_Rect CalculateTextRect(const std::string &str) const;
    };
    
} // namespace Renderer

#endif
