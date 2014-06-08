#include "surface_drawing.h"

#include <SDL2_gfxPrimitives.h>

#include <game/surface.h>

#include <game/rect.h>
#include <game/color.h>
#include <game/sdl_utils.h>
#include <game/sdl_error.h>

namespace Graphics
{
    template<
        /**
           function of signature
           void(*)(SDL_Renderer &renderer, const Rect &rect, const Color &color)
        **/
        class Painter
        >
    void CreateRendererAndPaint(Surface &surface, const Rect &rect, const Color &color, Painter painter)
    {
        const Rect clipRect = GetClipRect(surface);
        
        RendererPtr renderer(SDL_CreateSoftwareRenderer(surface));
        if(!renderer) {
            throw sdl_error();
        }
        
        if(SDL_RenderSetClipRect(renderer.get(), &clipRect) < 0) {
            throw sdl_error();
        }

        painter(*renderer, rect, color);
    }
    
    void DrawFrame(Surface &dst, const Rect &frame, const Color &color)
    {
        CreateRendererAndPaint(dst, frame, color, Render::DrawFrame);
    }

    void FillFrame(Surface &dst, const Rect &frame, const Color &color)
    {
        CreateRendererAndPaint(dst, frame, color, Render::FillFrame);
    }

    void DrawRhombus(Surface &dst, const Rect &bounds, const Color &color)
    {
        CreateRendererAndPaint(dst, bounds, color, Render::DrawRhombus);
    }

    void FillRhombus(Surface &dst, const Rect &bounds, const Color &color)
    {
        CreateRendererAndPaint(dst, bounds, color, Render::FillRhombus);
    }
}

namespace Render
{    
    void DrawFrame(SDL_Renderer &renderer, const Rect &frame, const Color &color)
    {
        SDL_SetRenderDrawBlendMode(&renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(&renderer, color.r, color.g, color.b, color.a);
        if(SDL_RenderDrawRect(&renderer, &frame) < 0) {
            throw sdl_error();
        }
    }

    void FillFrame(SDL_Renderer &renderer, const Rect &frame, const Color &color)
    {
        SDL_SetRenderDrawBlendMode(&renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(&renderer, color.r, color.g, color.b, color.a);
        if(SDL_RenderFillRect(&renderer, &frame) < 0) {
            throw sdl_error();
        }
    }

    template<
        /**
           polygonRGBA or filledPolygonRGBA
           or any other function of signature
           int(*)(SDL_Renderer*, Sint16 const*, Sint16 const*, int, Uint8, Uint8, Uint8, Uint8)
        **/
        class Painter
        >
    void PaintRhombus(SDL_Renderer &renderer, const Rect &bounds, const Color &color, Painter painter)
    {
        const Sint16 x1 = bounds.x;
        const Sint16 y1 = bounds.y;
        const Sint16 x2 = bounds.x + bounds.w;
        const Sint16 y2 = bounds.y + bounds.h;

        const Sint16 centerX = (x1 + x2) / 2;
        const Sint16 centerY = (y1 + y2) / 2;

        constexpr int NumPoints = 5;
        const Sint16 xs[NumPoints] = {x1,       centerX,    x2,         centerX,    x1};
        const Sint16 ys[NumPoints] = {centerY,  y1,         centerY,    y2,         centerY};
    
        painter(&renderer, &xs[0], &ys[0], NumPoints, color.r, color.g, color.b, color.a);
    }

    void DrawRhombus(SDL_Renderer &renderer, const Rect &bounds, const Color &color)
    {
        PaintRhombus(renderer, bounds, color, polygonRGBA);
    }

    void FillRhombus(SDL_Renderer &renderer, const Rect &bounds, const Color &color)
    {
        PaintRhombus(renderer, bounds, color, filledPolygonRGBA);
    }
}
