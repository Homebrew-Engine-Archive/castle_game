#include "main.h"

int main()
{
    SDLContext sdl(SDL_INIT_EVERYTHING);
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
    
    SDL_Window *wnd;
    SDL_Renderer *renderer;
    if(0 != SDL_CreateWindowAndRenderer(640, 480, 0, &wnd, &renderer))
        throw SDLError("SDL_CreateWindowAndRenderer");

    SDL_RWops *src = SDL_RWFromFile("/opt/stronghold/gm/tree_apple.gm1", "rb");
    GM1CollectionScheme scheme(src);
    DebugPrint_GM1Header(scheme.header);

    std::vector<Frame> frames;
    ReadFrameSet(src, scheme, frames);

    std::vector<Frame> slice;
    for(size_t i = 0; i < 24; ++i) {
        slice.push_back(frames[i]);
    }

    Animation anim(scheme.header, frames);

    bool quit = false;
    while(!quit) {
        SDL_Event e;
        SDL_PumpEvents();
        if(SDL_PollEvent(&e)) {
            switch(e.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                switch(e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                case SDLK_LEFT:
                    anim.Prev();
                    break;
                case SDLK_RIGHT:
                    anim.Next();
                    break;
                case SDLK_RETURN:
                    SDL_Log("# %d", anim.index);
                    DebugPrint_GM1ImageHeader(anim.CurrentFrame().header);
                    break;
                }
                break;
            }
        }
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                SDL_PushEvent(&e);
                break;
            }
        }

        auto &frame = anim.CurrentFrame();
        auto palette = scheme.palettes[1];
        auto texture  = frame.BuildTexture(renderer, palette);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_Rect src = anim.Box();
        SDL_Rect dst = anim.ShiftedBox(0, anim.CurrentOffset());

        int width, height;
        SDL_GetWindowSize(wnd, &width, &height);
        SDL_Rect screen;
        screen.x = 0;
        screen.y = 0;
        screen.w = width;
        screen.h = height;

        int posX = GetGM1AnchorX(scheme.header);
        int posY = GetGM1AnchorY(scheme.header);
        
        // if(width != (int)frame.width || height != (int)frame.height)
        //     SDL_SetWindowSize(wnd, frame.width, frame.height);

        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_RenderFillRect(renderer, &screen);
        // DrawIsmetricAxes(renderer, posX, posY, frame.width, frame.height);
        DrawFrontalAxes(renderer, posX, posY, width, height);
        SDL_RenderCopy(renderer, texture, &src, &dst);
        SDL_DestroyTexture(texture);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(wnd);
    
    return 0;
}

Animation::Animation(const GM1Header &header, const std::vector<Frame> &frames)
    : frames(frames)
    , index(0)
    , header(header)
{
    for(const auto &frame : frames) {
        offsets.push_back(EvalOffset(frame));
    }
}

size_t Animation::ShiftIndex(int n) const
{
    return (index + n + frames.size()) % frames.size();
}

size_t Animation::GetNextIndex() const
{
    return ShiftIndex(1);
}

size_t Animation::GetPrevIndex() const
{
    return ShiftIndex(-1);
}

void Animation::Next()
{
    index = GetNextIndex();
}

void Animation::Prev()
{
    index = GetPrevIndex();
}

const Frame& Animation::CurrentFrame() const
{
    return frames[index];
}

bool IsAnchoredPixel(Uint8 pixel)
{
    return (pixel != GM1_TGX8_TRANSPARENT_INDEX)
        && (pixel != GM1_TGX8_SHADOW_INDEX);
}

bool IsAnchoredLine(const Uint8 *scanline, Uint32 width)
{
    for(size_t i = 0; i < width; ++i) {
        if(IsAnchoredPixel(scanline[i]))
            return true;
    }
    return false;
}

int Animation::EvalOffset(const Frame &frame) const
{
    int anchorX = GetGM1AnchorX(header);
    int anchorY = GetGM1AnchorY(header);

    int offset = 0;
    for(int y = 0; y < anchorY; ++y) {
        const Uint8 *rowPixels = frame.buffer.data() + (anchorY - y) * frame.width;
        size_t pos = std::max(0, (int)(anchorX - TILE_RHOMBUS_WIDTH / 2));
        size_t size = std::min(TILE_RHOMBUS_WIDTH, frame.width - pos);
        if(IsAnchoredLine(rowPixels + pos, size)) {
            offset = y;
            break;
        }
    }

    return offset;
}

int Animation::CurrentOffset() const
{
    return offsets[index];
}

SDL_Rect Animation::Box() const
{
    SDL_Rect r;
    r.x = 0;
    r.y = 0;
    r.w = CurrentFrame().width;
    r.h = CurrentFrame().height;
    return r;
}

SDL_Rect Animation::ShiftedBox(int dx, int dy) const
{
    SDL_Rect r = Box();
    r.x += dx;
    r.y += dy;
    return r;
}

void DrawFrontalAxes(SDL_Renderer *renderer, int cx, int cy, int width, int height)
{
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, 0, cy, width, cy);
    SDL_RenderDrawLine(renderer, cx, 0, cx, height);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void DrawIsometricAxes(SDL_Renderer *renderer, int cx, int cy, int width, int height)
{
    int h1 = (std::abs(cx - width) * TILE_RHOMBUS_HEIGHT) / TILE_RHOMBUS_WIDTH;
    int h2 = (cx * TILE_RHOMBUS_HEIGHT) / TILE_RHOMBUS_WIDTH;
    SDL_RenderDrawLine(renderer, 0, cy - h2, cx, cy);
    SDL_RenderDrawLine(renderer, width, cy - h1, cx, cy);
}

template<class OutputIterator>
void LoadStringList(const std::string &filename, OutputIterator out)
{
    using namespace std;
    ifstream fin(filename);
    while(!fin.eof()) {
        string s;
        getline(fin, s);
        if(!s.empty())
            *out++ = s;
    }
}
