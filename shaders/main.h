#ifndef MAIN_H_
#define MAIN_H_

#include <cassert>
#include <cstring>

#include <chrono>
#include <vector>
#include <sstream>

#include <game/gm1reader.h>
#include <game/gm1entryreader.h>
#include <game/gm1.h>
#include <game/surface.h>
#include <game/sdl_utils.h>
#include <game/tgx.h>

#include <SDL.h>

#include <GL/glew.h>

#include "glerror.h"
#include "animationsheet.h"
#include "palettesheet.h"
#include "shaderprogram.h"
#include "misc.h"

namespace Shaders
{
    class Demo
    {
        SDLInitializer mInit;
        WindowPtr mWnd;
        
        Surface mImage;
        GM1::Palette mPalette;

        GLuint mRgb8Program = 0;
        GLuint mRgbProgram = 0;

        GLint mTextureCount = 1;

        GLuint mBackground = 0;
        GLuint mAnimationSheet = 0;
        GLuint mPaletteSheet = 0;

        GLuint mEntireVAO = 0;
        GLuint mVAO = 0;

        SDL_GLContext mContext;
        
    public:
        Demo();
        void InitBackground();
        void InitShaders();
        void InitTextures();
        void InitGeometry();
        int Main();
        void Render();
        void Reshape(int width, int height);
        void ReportStats(int msec, int frames);
        ~Demo();
    };
}

#endif // MAIN_H_
