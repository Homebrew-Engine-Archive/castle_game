#include "main.h"

using namespace std;

int main()
{
    SDLContext(SDL_INIT_EVERYTHING);
    SDLWindow window("SH", 1200, 768);
    SDLRenderer renderer(window);

    vector<string> files;
    LoadStringList("gm1list.txt", back_inserter(files));
    
    for(const auto &file : files) {
        SDL_Log("Reading %s", file.c_str());
        SDL_RWops *src = SDL_RWFromFile(file.c_str(), "rb");
        try {
            GM1CollectionScheme scheme(src);
            std::vector<GM1Entry> entries;
            LoadEntries(src, scheme, entries);
            std::shared_ptr<SDLSurface> surface =
                AllocGM1DrawingPlain(scheme);            
            for(size_t i = 0; i < entries.size(); ++i) {
                SDL_Rect r = MakeRect(
                    scheme.headers[i].posX,
                    scheme.headers[i].posY,
                    entries[i].surface->Width(),
                    entries[i].surface->Height());
                renderer.RegisterSurfaceRect(file, i, r);
                surface->Blit(*entries[i].surface, &r);
            }
            renderer.RegisterDrawingPlain(file, surface);
        } catch(const EOFError &eof) {
            SDL_Log("An error occured while reading %s", file.c_str());
            SDL_Log("\t%s", eof.what());
        } catch(const FormatError &fmt) {
            SDL_Log("An error occured while decoding %s", file.c_str());
            SDL_Log("\t%s", fmt.what());
        }        
        SDL_RWclose(src);
    }

    Game game;

    bool flag_Quit = false;
    bool flag_Initial = true;

    // Time when last frame was drawn
    Uint32 ms_EndLastFrame = 0;
    Uint32 ms_BeginLastFrame = 0;

    // Frames drawn since started
    Uint32 n_FrameCountTotal = 0;

    // Frames drawn last second
    Uint32 n_FrameCountLastSecond = 0;

    // Time when last second is over
    Uint32 ms_EndLastSecond = 0;

    // Desired frame rate
    Uint32 n_FrameRate = 30;

    // Adjusted interval between frames
    Uint32 ms_Interval = 1000 / n_FrameRate;

    // Used for simulation purposes
    Uint32 ms_LastTimeElapsed = 0;
    
    while(!flag_Quit) {
        bool flag_Redraw = false;
        Uint32 ms_CurrentTime = SDL_GetTicks();

        // Drop last second's frame counter
        if(ms_EndLastSecond + 1000 < ms_CurrentTime) {
            SDL_Log("FPS: %d", n_FrameCountLastSecond);
            
            n_FrameCountLastSecond = 0;
            ms_EndLastSecond = ms_CurrentTime;
        }

        // Check if there is need to draw frame
        if(ms_BeginLastFrame + ms_Interval < ms_CurrentTime) {
            flag_Redraw = true;
        }

        if(flag_Initial) {
            flag_Initial = false;
            flag_Redraw = true;
            ms_LastTimeElapsed = ms_CurrentTime;
        }

        Uint32 ms_UpToNextFrame;
        if(ms_CurrentTime <= ms_BeginLastFrame + ms_Interval) {
            ms_UpToNextFrame = 
                (ms_BeginLastFrame + ms_Interval) - ms_CurrentTime;
        } else {
            ms_UpToNextFrame = 0;
        }
        
        SDL_Event e;
        SDL_PumpEvents();
        if(SDL_WaitEventTimeout(&e, ms_UpToNextFrame)) {
            switch(e.type) {
            case SDL_QUIT:
                game.OnQuit();
                flag_Quit = true;
                break;
            case SDL_KEYDOWN:
                game.OnKeyDown(e.key);
                break;
            case SDL_KEYUP:
                game.OnKeyUp(e.key);
                break;
            case SDL_MOUSEBUTTONDOWN:
                game.OnMouseButtonDown(e.button);
                break;
            case SDL_MOUSEBUTTONUP:
                game.OnMouseButtonUp(e.button);
                break;
            case SDL_MOUSEMOTION:
                game.OnMouseMotion(e.motion);
                break;
            }
        }

        game.OnTimeElapsed(ms_CurrentTime - ms_LastTimeElapsed);
        ms_LastTimeElapsed = ms_CurrentTime;
        
        if(flag_Redraw) {
            ms_BeginLastFrame = ms_CurrentTime;
            game.OnFrame(renderer);
            
            ++n_FrameCountTotal;
            ++n_FrameCountLastSecond;

            renderer.Present();
            ms_EndLastFrame = SDL_GetTicks();
        }

        if(game.IsClosed())
            flag_Quit = true;
    }
    
    return 0;
}

template<class OutputIterator>
void LoadStringList(const char * filename, OutputIterator out)
{
    ifstream fin(filename);
    string line;
    while(getline(fin, line))
        *out++ = line;
}
