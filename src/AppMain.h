#ifndef CAPPMAIN_H_
#define CAPPMAIN_H_

#include <iterator>
#include <fstream>
#include <initializer_list>
#include <vector>
#include <string>
#include <algorithm>

#include <SDL2/SDL.h>
#include "Exceptions.h"
#include "SDLContext.h"

#include "TGX.h"

class CAppMain
{
private:
    void HandleSingleEvent(const SDL_Event &e);
    SDL_Texture *LoadTGXTexture(const std::string &fn);
    void LoadTGXList(const std::string &filename);
    
public:
    CAppMain(const SDLContext &, int argc, const char* argv[]);
    ~CAppMain();
    void MainLoop();

private:
    bool m_quit;
    SDL_Window *m_wnd;
    SDL_Renderer *m_render;
    SDL_Texture *m_texture;
    std::vector<std::string> m_tgxList;

};

#endif
