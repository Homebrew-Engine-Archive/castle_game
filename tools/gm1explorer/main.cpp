#include "main.h"

using namespace std;

static void CheckSDLError(int code)
{
    if(code < 0)
        throw std::runtime_error(SDL_GetError());
}

template<class T, class D>
static void CheckSDLError(const std::unique_ptr<T, D> &ptr)
{
    if(!ptr)
        throw std::runtime_error(SDL_GetError());
}

static void CheckSDLError(SDL_bool boolvalue)
{
    if(boolvalue == SDL_FALSE)
        throw std::runtime_error(SDL_GetError());
}

int main(int argc, const char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    if(argc > 1) {
        std::string filename = argv[1];
        return explorer_main(filename);
    } else {
        std::cout << "Usage: "
                  << argv[0]
                  << " <gm1file>"
                  << std::endl;
    }
    SDL_Quit();
    return 0;
}

int explorer_main(const std::string &filename)
{
    try {
        // Only open for header 
        RWPtr src(SDL_RWFromFile(filename.c_str(), "rb"));
        CheckSDLError(src);
        
        gm1::Collection collection(src.get());
        // Buffer all the rest
        FileBuffer rest(src.get());
        viewer_t viewer(collection, rest);
        viewer.enter_event_loop();
        return 0;
        
    } catch(const std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
        return -1;
    }
}

void print_pixel_format(const SDL_PixelFormat *pf)
{
    Uint32 rmask;
    Uint32 gmask;
    Uint32 bmask;
    Uint32 amask;
    int bpp;

    if(pf == NULL)
        throw std::runtime_error("passed NULL pixel format");
    
    CheckSDLError(
        SDL_PixelFormatEnumToMasks(pf->format, &bpp, &rmask, &gmask, &bmask, &amask));

    std::cout << std::hex
              << rmask << ' '
              << gmask << ' '
              << bmask << ' '
              << amask << ' '
              << std::dec
              << bpp << std::endl;
}

void decode_glyph(Surface &surface)
{
    auto swap_green_alpha = [](Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
        return SDL_Color {r, 255, b, g};
    };
    MapSurface(surface, swap_green_alpha);
}

Surface DecodeFont(Surface surface)
{
    Surface tmp = SDL_ConvertSurfaceFormat(
        surface, SDL_PIXELFORMAT_ARGB8888, 0);
    decode_glyph(tmp);
    return tmp;
}

viewer_t::viewer_t(const gm1::Collection &gm1, const FileBuffer &fb)
    : paletteIndex(0)
    , imageIndex(0)
    , data(fb)
    , collection(gm1)
    , quit(false)
    , frameLimit(true)
    , frameRate(24)
    , screenWidth(1366)
    , screenHeight(768)
    , showGreenRect(false)
    , showRedRect(false)
    , showAllRects(false)
    , showSingleImage(false)
    , selected { 0, 0, 0, 0 }
{
    alloc_window();
    alloc_renderer();
    print_renderer_info();

    gm1::PrintHeader(std::clog, gm1.header);
    RWPtr src(RWFromFileBuffer(fb));
    CheckSDLError(src);

    gm1::LoadEntries(src.get(), gm1, surfaces);
    if(gm1.encoding() == gm1::Encoding::Font) {
        for(auto &surface : surfaces) {
            surface = DecodeFont(surface);
        }
    }
    atlas = MakeSurfaceAtlas(surfaces, partitions, 2000, 8192);
    if(atlas.Null())
        throw std::runtime_error("Unable to build atlas");
    SDL_SetColorKey(atlas, SDL_TRUE, TGX_TRANSPARENT_RGB16);
     
    // Surface tmp = gm1::LoadAtlas(src.get(), gm1);
    // if(gm1.encoding() == gm1::Encoding::Font) {
    //     atlas = DecodeFont(tmp);
    // } else {
    //     atlas = tmp;
    // }
    
    gm1::PartitionAtlas(gm1, partitions);
    
    std::clog << "Atlas width: "
              << atlas->w
              << ' '
              << "Atlas height: "
              << atlas->h
              << ' '
              << "Bytes: "
              << atlas->w * atlas->h * 4
              << std::endl;

    alloc_texture(screenWidth, screenHeight);
    alloc_palettes();
    set_cursor(0, 0);

}

void viewer_t::draw()
{
    update_texture(streamTexture.get());

    CheckSDLError(
        SDL_RenderCopy(renderer.get(), streamTexture.get(), NULL, NULL));
            
    SDL_RenderPresent(renderer.get());
}

void viewer_t::update_texture(SDL_Texture *texture)
{
    int nativePitch;
    void *nativePixels;
    Uint32 format;
    TextureLocker lock(texture, NULL, &nativePixels, &nativePitch);

    int width;
    int height;
    CheckSDLError(
        SDL_QueryTexture(texture, &format, NULL, &width, &height));
    
    Uint32 rmask;
    Uint32 gmask;
    Uint32 bmask;
    Uint32 amask;
    int bpp;

    CheckSDLError(
        SDL_PixelFormatEnumToMasks(format, &bpp, &rmask, &gmask, &bmask, &amask));
    
    Surface dst = SDL_CreateRGBSurfaceFrom(
        nativePixels, width, height, bpp, nativePitch,
        rmask, gmask, bmask, amask);
    if(dst.Null())
        throw std::runtime_error(SDL_GetError());

    SDL_FillRect(dst, NULL, 0xff1f1f1f);
    
    if(HasPalette(atlas)) {
        CheckSDLError(
            SDL_SetSurfacePalette(atlas, palettes.at(paletteIndex).get()));
    }

    update_surface(dst);

}

void viewer_t::update_surface(Surface dst)
{
    draw_entry(dst, 100, 100, collection, imageIndex);

    RendererPtr sw =
        RendererPtr(
            SDL_CreateSoftwareRenderer(dst));

    if(showAllRects) {
        srand(0);
        for(const auto &rect : partitions) {
            SDL_SetRenderDrawColor(sw.get(), rand(), rand(), rand(), 255);
            SDL_Rect shifted = ShiftRect(rect, -offsetx, -offsety);
            SDL_RenderDrawRect(sw.get(), &shifted);
        }
    }

    if(showRedRect) {
        SDL_SetRenderDrawColor(sw.get(), 255, 0, 0, 255);

        SDL_Rect shifted = ShiftRect(partitions.at(imageIndex), -offsetx, -offsety);
        SDL_RenderDrawRect(sw.get(), &shifted);
    }

    if(showGreenRect) {
        SDL_SetRenderDrawColor(sw.get(), 0, 255, 0, 255);
        
        SDL_Rect shifted = ShiftRect(selected, -offsetx, -offsety);
        SDL_RenderDrawRect(sw.get(), &shifted);
    }
}

void viewer_t::draw_entry(Surface dst, int x, int y, const gm1::Collection &gm1, size_t index)
{
    SDL_Rect screen = ShiftRect(MakeRect(screenWidth, screenHeight), -offsetx, -offsety);
    SDL_Rect srcrect = partitions.at(index);

    int tileY = collection.headers.at(index).tileY;

    SDL_Rect dstrect = MakeRect(x, y - tileY, srcrect.w, srcrect.h);
    
    BlitSurface(surfaces.at(index), NULL, dst, &dstrect);
}

void viewer_t::handle_key(SDL_Keycode code)
{
    const int speed = 15;
    switch(code) {
    case SDLK_RETURN:
        std::cout << imageIndex << std::endl;
        gm1::PrintImageHeader(std::clog, collection.headers.at(imageIndex));
        break;
    case SDLK_ESCAPE:
        quit = true;
        break;
    case SDLK_RIGHT:
        set_cursor(1, 0);
        offsetx += speed;
        break;
    case SDLK_LEFT:
        set_cursor(-1, 0);
        offsetx -= speed;
        break;
    case SDLK_UP:
        set_cursor(0, 1);
        offsety -= speed;
        break;
    case SDLK_DOWN:
        set_cursor(0, -1);
        offsety += speed;
        break;
    case SDLK_SPACE:
        frameLimit = !frameLimit;
        std::cout << "FrameLimit: " << frameLimit << std::endl;
        break;
    case SDLK_j:
        frameRate += 1;
        std::cout << "FrameRate: " << frameRate << std::endl;
        break;
    case SDLK_k:
        frameRate -= 1;
        std::cout << "FrameRate: " << frameRate << std::endl;
        break;
    }
}

void viewer_t::print_renderer_info()
{
    SDL_RendererInfo info;
    CheckSDLError(
        SDL_GetRendererInfo(renderer.get(), &info));

    std::cout << "Renderer: " << info.name << std::endl;
}

void viewer_t::alloc_window()
{
    window = WindowPtr(
        SDL_CreateWindow("View", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED,
                         screenWidth,
                         screenHeight,
                         SDL_WINDOW_RESIZABLE));
    CheckSDLError(window);
}

void viewer_t::alloc_renderer()
{
    renderer =
        RendererPtr(
            SDL_CreateRenderer(window.get(), -1, 0));
    CheckSDLError(renderer);
}

void viewer_t::alloc_palettes()
{
    palettes.resize(GM1_PALETTE_COUNT);
    for(size_t i = 0; i < GM1_PALETTE_COUNT; ++i) {
        palettes.at(i) = gm1::CreateSDLPaletteFrom(collection.palettes.at(i));
    }
}

void viewer_t::alloc_texture(int width, int height)
{
    streamTexture =
        TexturePtr(
            SDL_CreateTexture(
                renderer.get(),
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                width, height));
    CheckSDLError(streamTexture);
}
    
void viewer_t::enter_event_loop()
{
    int secondEnd = 0;
    int fps = 0;
        
    while(!quit) {                
        const int frameStart = SDL_GetTicks();
        if(secondEnd + 1000 < frameStart) {
            secondEnd = frameStart;
            //std::cout << "FPS: " << fps << std::endl;
            fps = 0;
        }

        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            switch(e.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                handle_key(e.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                handle_mouse_down(e.button.x, e.button.y);
                break;
            }
        }

        draw();
        fps++;
        
        const int frameEnd = SDL_GetTicks();
        const int frameInterval = 1000 / frameRate;
        if(frameLimit && (frameEnd - frameStart) < frameInterval) {
            SDL_Delay(frameInterval - (frameEnd - frameStart));
        }
    }
}

SDL_Palette *viewer_t::get_palette_by_index(size_t index)
{
    return palettes.at(index).get();
}
    
size_t viewer_t::cycle_ref(size_t index, size_t max, int d)
{
    return ((int)index + (int)max + d) % max;
}

void viewer_t::set_cursor(int ii, int pi)
{
    imageIndex = cycle_ref(imageIndex, collection.size(), ii);
    paletteIndex = cycle_ref(paletteIndex, GM1_PALETTE_COUNT, pi);
}
    
void viewer_t::handle_mouse_down(int x, int y)
{
    for(const auto &rect : partitions) {
        if(IsInRect(rect, x + offsetx, y + offsety)) {
            selected = rect;
            std::cout << selected << std::endl;
        }
    }
}
