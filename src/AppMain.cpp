#include "AppMain.h"

CAppMain::CAppMain(const SDLContext &context, int argc, const char *argv[])
{
    if(0 != SDL_CreateWindowAndRenderer(1024, 768, 0, &m_wnd, &m_render))
        throw SDLError("SDL_CreateWindowAndRenderer");
    LoadTGXList("textures.txt");
    srand(time(NULL));
    const char *filename = m_tgxList[rand() % m_tgxList.size()].c_str();
    SDL_Log("Loading texture: %s\n", filename);
    m_texture = LoadTGXTexture(filename);
    if(m_texture == NULL)
        throw SDLError("LoadTGXTexture");
}

CAppMain::~CAppMain()
{
    SDL_DestroyRenderer(m_render);
    SDL_DestroyWindow(m_wnd);
}

void CAppMain::MainLoop()
{
    SDL_Event e;
    while(!m_quit) {
        SDL_PumpEvents();
        while(SDL_PollEvent(&e)) {
            HandleSingleEvent(e);
        }
        if(m_texture != NULL) {
            if(0 != SDL_RenderCopy(m_render, m_texture, NULL, NULL))
                throw SDLError("SDL_RenderCopy");
            SDL_RenderPresent(m_render);
        }
        SDL_Delay(100);
    }
}

void CAppMain::HandleSingleEvent(const SDL_Event &e)
{
    switch(e.type) {
    case SDL_QUIT:
        m_quit = true;
        return;
    case SDL_KEYDOWN:
        switch(e.key.keysym.sym) {
        case SDLK_ESCAPE:
            m_quit = true;
            return;
        default:
            return;
        }
        return;
    default:
        return;
    }
}

void CAppMain::LoadTGXList(const std::string &filename)
{
    std::ifstream fin(filename);
    while(!fin.eof()) {
        std::string tgxFilename;
        std::getline(fin, tgxFilename);
        m_tgxList.push_back(tgxFilename);
    }
}

SDL_Texture *CAppMain::LoadTGXTexture(const std::string &fn)
{
    SDL_Texture *texture = NULL;
    SDL_Surface *surf = NULL;
    SDL_RWops *io = NULL;
    Uint16 *bits = NULL;
    try {
        io = SDL_RWFromFile(fn.c_str(), "rb");
        if(io == NULL)
            throw SDLError("SDL_RWFromFile");
    
        TGXHeader hdr;
        if(0 != ReadTGXHeader(io, &hdr)) {
            throw ReadTGXError("ReadTGXHeader failed");
        }
        
        if((hdr.width > MAX_TGX_WIDTH) || (hdr.height > MAX_TGX_HEIGHT)) {
            throw ReadTGXError("Image size is too big.");
        }
        
        bits = new Uint16[hdr.width * hdr.height];
        std::fill(bits, bits + hdr.width * hdr.height, 0);
        if(0 != ReadTGXPixelsNoPalette(io, hdr.width, hdr.height, bits))
            throw ReadTGXError("ReadTGXPixelsNoPalette failed.");
        
        surf = SDL_CreateRGBSurfaceFrom(
            bits, hdr.width, hdr.height, 16,
            hdr.width * sizeof(*bits),
            0x7c00, 0x3e0, 0x1f, 0);
        
        if(surf == NULL)
            throw SDLError("SDL_CreateRGBSurfaceFrom");

        texture = SDL_CreateTextureFromSurface(m_render, surf);
        if(texture == NULL)
            throw SDLError("SDL_CreateTextureFromSurface");

    } catch(const ReadTGXError &e) {
        SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Read TGX failed: %s\n",
                        e.what());
    } catch(const SDLError &e) {
        SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "SDL Error in %s: %s\n",
                        e.where(), e.what());
    } catch(const std::exception &e) {
        SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Unknown exception: %s\n",
                        e.what());
    }

    if(surf != NULL)
        SDL_FreeSurface(surf);
        
    if(bits != NULL)
        delete [] bits;

    if(io != NULL)
        io->close(io);

    return texture;
}
