#include "gamescreen.h"
#include "landscape.h"
#include "geometry.h"
#include "filesystem.h"

GameScreen::GameScreen(RootScreen *root)
    : m_root(root)
    , m_renderer(root->GetRenderer())
    , m_cursorX(0)
    , m_cursorY(0)
    , m_cursorInvalid(true)
    , m_viewportX(0)
    , m_viewportY(0)
    , m_viewportRadius(1)
    , m_viewportOrient(Orient::Front)
    , m_flatView(false)
    , m_lowView(false)
    , m_zoomedOut(false)
    , m_hiddenUI(false)
    , m_cursorMode(CursorMode::Normal)
{
}

GameScreen::~GameScreen()
{
}

void GameScreen::Draw(Surface &frame)
{
    SDL_Rect frameRect = SurfaceBounds(frame);
    SDL_FillRect(frame, &frameRect, 0xff000000);

    FilePath filepath = GetGM1FilePath("body_woodcutter");
    const CollectionData &gm1 = m_renderer->QueryCollection(filepath);
    
    for(size_t n = 0; n < 1000; ++n) {
        size_t paletteIndex = 1 + rand() % (gm1.palettes.size() - 2);
        SDL_Palette *palette = gm1.palettes.at(paletteIndex).get();
        const CollectionEntry &entry =
            gm1.entries[rand() % gm1.header.imageCount];
        Surface surface = entry.surface;
        SDL_SetSurfacePalette(surface, palette);
        SDL_Rect whither = MakeRect(
            rand() % (frameRect.w - surface->w),
            rand() % (frameRect.h - surface->h),
            surface->w,
            surface->h);
        BlitSurface(surface, NULL, frame, &whither);
    }
}

bool GameScreen::HandleEvent(const SDL_Event &event)
{
    switch(event.type) {
    case SDL_MOUSEMOTION:
        {
            m_cursorInvalid = false;
            m_cursorX = event.motion.x;
            m_cursorY = event.motion.y;
        }
        break;
    case SDL_KEYDOWN:
        {
            switch(event.key.keysym.sym) {
            case SDLK_ESCAPE:
                return false;
            default:
                return true;
            }
        }
        break;
    }
    return false;
}

void GameScreen::AdjustViewport(const SDL_Rect &screen)
{
    if(screen.w == m_cursorX)
        ++m_viewportX;
    else if(0 == m_cursorX)
        --m_viewportX;

    if(screen.h == m_cursorY)
        ++m_viewportY;
    else if(0 == m_cursorY)
        --m_viewportY;
}


Orient NextRotation(Orient rot)
{
    switch(rot) {
    case Orient::Front: return Orient::Left;
    case Orient::Left: return Orient::Back;
    case Orient::Back: return Orient::Right;
    case Orient::Right: return Orient::Front;
    default: return Orient::Front;
    }
}

Orient PrevRotation(Orient rot)
{
    switch(rot) {
    case Orient::Left: return Orient::Front;
    case Orient::Front: return Orient::Right;
    case Orient::Right: return Orient::Back;
    case Orient::Back: return Orient::Left;
    default: return Orient::Front;
    }
}
