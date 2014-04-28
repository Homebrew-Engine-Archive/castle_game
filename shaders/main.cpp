#include "main.h"

int main()
{
    try {
        Shaders::Demo demo;
        return demo.Main();
    } catch(const std::exception &error) {
        std::cerr << error.what() << std::endl;
    }
    return -1;
}

namespace
{
    GLuint LoadImage16(const Surface &surface)
    {
        static const GLint NumMipmaps = 1;
        static const GLint XOffset = 0;
        static const GLint YOffset = 0;
        const GLint width = surface->w;
        const GLint height = surface->h;

        GLuint tex = 0;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexStorage2D(GL_TEXTURE_2D, NumMipmaps, GL_RGBA, width, height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        const SurfaceLocker locker(surface);
        glTexSubImage2D(GL_TEXTURE_2D,
                        NumMipmaps-1,
                        XOffset,
                        YOffset,
                        width,
                        height,
                        GL_BGRA,
                        GL_UNSIGNED_SHORT_1_5_5_5_REV,
                        surface->pixels);
        return tex;
    }
}

namespace Shaders
{
    Demo::Demo()
    {
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        mWnd = WindowPtr(
            SDL_CreateWindow("Palette shaders", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE));
        if(!mWnd) {
            throw std::runtime_error(SDL_GetError());
        }

        mContext = SDL_GL_CreateContext(mWnd.get());
        if(!mContext) {
            throw std::runtime_error(SDL_GetError());
        }

        glewInit();
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);
        glCullFace(GL_FRONT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        InitShaders();
        InitTextures();
        InitGeometry();
    }
    
    void Demo::InitShaders()
    {
        try {
            mRgb8Program = Shaders::LinkProgram({
                Shaders::CompileShader(GL_VERTEX_SHADER, Misc::LoadString<GLchar>("rgb8.vert")),
                Shaders::CompileShader(GL_FRAGMENT_SHADER, Misc::LoadString<GLchar>("rgb8.frag"))
            });

            glUniform1i(glGetUniformLocation(mRgb8Program, "indexed"), 0);
            glUniform1i(glGetUniformLocation(mRgb8Program, "palette"), 1);

            mRgbProgram = Shaders::LinkProgram({
                Shaders::CompileShader(GL_VERTEX_SHADER, Misc::LoadString<GLchar>("rgb.vert")),
                Shaders::CompileShader(GL_FRAGMENT_SHADER, Misc::LoadString<GLchar>("rgb.frag"))
            });

            glUniform1i(glGetUniformLocation(mRgbProgram, "image"), 0);
            
        } catch(const std::exception &error) {
            std::cerr << "Error loading shader: " << error.what() << std::endl;
            throw;
        }
    }

    void Demo::InitBackground()
    {
        try {
            std::ifstream fin("/opt/stronghold/gfx/frontend_main.tgx", std::ios_base::binary);
            mBackground = LoadImage16(
                TGX::ReadTGX(fin));
        } catch(const std::exception &error) {
            std::cerr << "Error loading background: " << error.what() << std::endl;
            throw;
        }
    }
    
    void Demo::InitTextures()
    {
        try {
            GM1::GM1Reader gm1("/opt/stronghold/gm/body_archer.gm1");

            mAnimationSheet = Shaders::LoadAnimationSheet(gm1);
            
            mTextureCount = gm1.NumEntries();
            std::vector<GM1::Palette> palettes;
            for(int i = 0; i < gm1.NumPalettes(); ++i) {
                palettes.push_back(
                    gm1.Palette(i));
            }
            mPaletteSheet = LoadPaletteSheet(palettes);

            InitBackground();
        } catch(const std::exception &error) {
            std::cerr << "Error loading textures: " << error.what() << std::endl;
            throw;
        }
    }
    
    struct Vec2
    {
        GLfloat x;
        GLfloat y;
    };
    
    struct VertexData
    {
        GLfloat x;
        GLfloat y;
        GLfloat s;
        GLfloat t;
    };

    void Demo::InitGeometry()
    {
        try {
            static VertexData sprites[] = {
                {1.0f, 0.0f, 1.0f, 1.0f},
                {1.0f, 1.0f, 1.0f, -1.0f},
                {0.0f, 1.0f, -1.0f, -1.0f},
                {0.0f, 0.0f, -1.0f, 1.0f}
            };
        
            // glGenVertexArrays(1, &mEntireVAO);
            // glBindVertexArray(mEntireVAO);
            
            // glGenVertexArrays(1, &mVAO);
            // glBindVertexArray(mVAO);

            GLuint buf = 0;
            glGenBuffers(1, &buf);
            glBindBuffer(GL_ARRAY_BUFFER, buf);
            glBufferData(GL_ARRAY_BUFFER, sizeof(sprites), sprites, GL_STATIC_DRAW);

            GLint posAttrib = glGetAttribLocation(mRgb8Program, "pos");
            glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (GLvoid*)offsetof(VertexData, x));
            glEnableVertexAttribArray(posAttrib);

            GLint texAttrib = glGetAttribLocation(mRgb8Program, "tex");
            glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (GLvoid*)offsetof(VertexData, s));
            glEnableVertexAttribArray(texAttrib);
        } catch(const std::exception &error) {
            std::cerr << "Error while loading geometry: " << error.what() << std::endl;
            throw;
        }
    }
    
    void Demo::Reshape(int width, int height)
    {
        glViewport(0, 0, width, height);
    }
    
    void Demo::ReportStats(int /* msec*/, int frames)
    {
        std::cout << "Rendered " << frames << std::endl;
    }

    int Demo::Main()
    {
        using namespace std::chrono;
        uint64_t fpsCounter = 0;
        bool quit = false;
        steady_clock::time_point lastSec = steady_clock::now();

        while(!quit) {
            SDL_Event event;
            while(SDL_PollEvent(&event)) {
                if(event.type == SDL_QUIT) {
                    quit = true;
                }
                if(event.type == SDL_WINDOWEVENT) {
                    if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        Reshape(event.window.data1, event.window.data2);
                    }
                }
            }

            steady_clock::time_point now = steady_clock::now();
            milliseconds elapsedMsec = duration_cast<milliseconds>(now - lastSec);
            seconds elapsedSec = duration_cast<seconds>(elapsedMsec);
            if(elapsedSec.count() != 0) {
                ReportStats(elapsedMsec.count(), fpsCounter);
                lastSec = now;
                fpsCounter = 0;
            }
            ++fpsCounter;
            Render();
        }
        return 0;
    }

    void Demo::Render()
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(mRgbProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mBackground);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glUseProgram(mRgb8Program);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, mAnimationSheet);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_1D_ARRAY, mPaletteSheet);

        for(int i = 0; i < 100; ++i) {
            glUniform1i(glGetUniformLocation(mRgb8Program, "texture_index"),
                        (rand() % mTextureCount));
            glUniform1i(glGetUniformLocation(mRgb8Program, "palette_index"),
                        (rand() % GM1::CollectionPaletteCount));
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
        
        SDL_GL_SwapWindow(mWnd.get());
    }

    Demo::~Demo()
    {
        // todo cleanup
    }
}
