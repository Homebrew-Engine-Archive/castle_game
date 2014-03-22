#include "gamescreen.h"
#include "landscape.h"
#include "geometry.h"
#include "filesystem.h"
#include "surface.h"
#include "collection.h"
#include "direction.h"

enum class BuildingCategory : int {
    Castle,
    Industry,
    Farming,
    Weaponary,
    Civil,
    Production
};

GameScreen::GameScreen(Castle::Engine *root)
    : mRoot(root)
    , mRenderer(root->GetRenderer())
    , mCursorX(0)
    , mCursorY(0)
    , mCursorInvalid(true)
    , mViewportX(0)
    , mViewportY(0)
    , mViewportRadius(1)
    , mViewportOrient(Direction::North)
    , mFlatView(false)
    , mLowView(false)
    , mZoomedOut(false)
    , mHiddenUI(false)
    , mCursorMode(CursorMode::Normal)
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
    const CollectionData &gm1 = mRenderer->QueryCollection(filepath);
    
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
            mCursorInvalid = false;
            mCursorX = event.motion.x;
            mCursorY = event.motion.y;
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
    if(screen.w == mCursorX)
        ++mViewportX;
    else if(0 == mCursorX)
        --mViewportX;

    if(screen.h == mCursorY)
        ++mViewportY;
    else if(0 == mCursorY)
        --mViewportY;
}
