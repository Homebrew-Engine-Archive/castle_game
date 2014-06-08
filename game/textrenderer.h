#ifndef TEXTRENDERER_H_
#define TEXTRENDERER_H_

#include <vector>
#include <string>

#include <SDL_ttf.h>

#include <game/font.h>
#include <game/color.h>
#include <game/point.h>
#include <game/rect.h>
#include <game/sdl_utils.h>
#include <game/alignment.h>

class Surface;

namespace Render
{    
    class TextRenderer
    {       
    public:
        explicit TextRenderer(Surface &surface);

        void SetFont(const UI::Font &font);
        const UI::Font GetFont() const;
        const Rect TextSize(const std::string &str) const;
        void PutString(const std::string &str, const Rect &textArea, const Color &facecolor);

    protected:
        Surface &mSurface;
        UI::Font mTextFont;
        TTF_Font *mCurrentFont;
        
    };    
} // namespace Renderer

#endif
