#include "main.h"

using namespace std;


void print_pixel_format(const SDL_PixelFormat *pf)
{
    Uint32 rmask;
    Uint32 gmask;
    Uint32 bmask;
    Uint32 amask;
    int bpp;

    if(pf == NULL)
        throw std::runtime_error("passed NULL pixel format");
    
    if(!SDL_PixelFormatEnumToMasks(pf->format, &bpp, &rmask, &gmask, &bmask, &amask))
        throw std::runtime_error(SDL_GetError());

    std::cout << std::hex
              << rmask << ' '
              << gmask << ' '
              << bmask << ' '
              << amask << ' '
              << std::dec
              << bpp << std::endl;
}


Uint32 decode_glyph_pixel(const SDL_PixelFormat *format, Uint32 color)
{
    Uint8 r, g, b, a;
    SDL_GetRGBA(color, format, &r, &g, &b, &a);
    return SDL_MapRGBA(format, r, 255, b, g);
}

void decode_glyph(Surface &surface)
{
    if(surface.Null())
        throw std::invalid_argument("passed surface is NULL");
    SurfaceLocker lock(surface);
    
    Uint8 *pixels = (Uint8 *)surface->pixels;
    int pitch = surface->pitch;
    int height = surface->h;
    int width = surface->w;
    SDL_PixelFormat *format = surface->format;
    print_pixel_format(format);

    for(int i = 0; i < height; ++i) {
        Uint32 *row = (Uint32 *)(pixels + pitch * i);
        for(int j = 0; j < width; ++j) {
            row[j] = decode_glyph_pixel(format, row[j]);
        }
    }
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
{    
    alloc_window();
    alloc_renderer();
    alloc_texture();
    alloc_palettes();
    set_cursor(0, 0);
    
    print_renderer_info();
    
    scoped_rwops src(RWFromFileBuffer(fb));
    Surface tmp = gm1::LoadAtlas(src.get(), gm1);
    atlas = SDL_ConvertSurfaceFormat(
        tmp, SDL_PIXELFORMAT_ARGB8888, 0);
    decode_glyph(atlas);
}
    
viewer_t::~viewer_t()
{
    dealloc_texture();
    dealloc_palettes();
    dealloc_renderer();
    dealloc_window();
}

void viewer_t::print_renderer_info()
{
    SDL_RendererInfo info;
    if(SDL_GetRendererInfo(renderer, &info))
        throw std::runtime_error(SDL_GetError());

    SDL_Log("Renderer: %s", info.name);
}

void viewer_t::alloc_window()
{
    window = SDL_CreateWindow("View", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              screenWidth,
                              screenHeight,
                              SDL_WINDOW_RESIZABLE);
    if(window == NULL)
        throw std::runtime_error(SDL_GetError());
}

void viewer_t::alloc_renderer()
{
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(renderer == NULL)
        throw std::runtime_error(SDL_GetError());
}

void viewer_t::dealloc_window()
{
    if(window != NULL)
        SDL_DestroyWindow(window);
}

void viewer_t::dealloc_renderer()
{
    if(renderer != NULL)
        SDL_DestroyRenderer(renderer);
}

void viewer_t::alloc_palettes()
{
    palettes.resize(GM1_PALETTE_COUNT);
    for(size_t i = 0; i < GM1_PALETTE_COUNT; ++i) {
        palettes.at(i) = gm1::CreateSDLPaletteFrom(collection.palettes.at(i));
    }
}

void viewer_t::dealloc_palettes()
{
    for(SDL_Palette *palette : palettes) {
        if(palette != NULL)
            SDL_FreePalette(palette);
    }
}

void viewer_t::alloc_texture()
{
    Uint32 format = SDL_MasksToPixelFormatEnum(
        16, TGX_RGB16_RMASK,
        TGX_RGB16_GMASK,
        TGX_RGB16_BMASK,
        TGX_RGB16_AMASK);
    streamTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        screenWidth, screenHeight);
    if(streamTexture == NULL)
        throw std::runtime_error(SDL_GetError());
}

void viewer_t::dealloc_texture()
{
    if(streamTexture != NULL) {
        SDL_DestroyTexture(streamTexture);
    }
}
    
void viewer_t::enter_event_loop()
{
    int secondEnd = 0;
    int fps = 0;
        
    while(!quit) {                
        const int frameStart = SDL_GetTicks();
        if(secondEnd + 1000 < frameStart) {
            secondEnd = frameStart;
            SDL_Log("FPS: %d", fps);
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
    return palettes.at(index);
}
    
size_t viewer_t::cycle_ref(size_t index, size_t max, int d)
{
    return ((int)index + (int)max + d) % max;
}

void viewer_t::set_cursor(int ii, int pi)
{
    imageIndex = cycle_ref(imageIndex, collection.header.imageCount, ii);
    paletteIndex = cycle_ref(paletteIndex, GM1_PALETTE_COUNT, pi);
}
    
void viewer_t::handle_key(SDL_Keycode code)
{
    switch(code) {
    case SDLK_RETURN:
        std::cout << imageIndex << std::endl;
        gm1::VerbosePrintImageHeader(collection.headers.at(imageIndex));
        break;
    case SDLK_ESCAPE:
        quit = true;
        break;
    case SDLK_RIGHT:
        set_cursor(1, 0);
        break;
    case SDLK_LEFT:
        set_cursor(-1, 0);
        break;
    case SDLK_UP:
        set_cursor(0, 1);
        break;
    case SDLK_DOWN:
        set_cursor(0, -1);
        break;
    case SDLK_SPACE:
        frameLimit = !frameLimit;
        SDL_Log("FrameLimit: %d", frameLimit);
        break;
    case SDLK_j:
        frameRate += 1;
        SDL_Log("FrameRate: %d", frameRate);
        break;
    case SDLK_k:
        frameRate -= 1;
        SDL_Log("FrameRate: %d", frameRate);
        break;
    }
}

void viewer_t::draw()
{
    update_texture(streamTexture);

    if(SDL_RenderCopy(renderer, streamTexture, NULL, NULL) < 0)
        throw std::runtime_error(SDL_GetError());
        
    SDL_RenderPresent(renderer);
}

void viewer_t::update_texture(SDL_Texture *texture)
{
    int nativePitch;
    void *nativePixels;
    Uint32 format;
    TextureLocker lock(texture, NULL, &nativePixels, &nativePitch);

    if(SDL_QueryTexture(texture, &format, NULL, NULL, NULL))
        throw std::runtime_error(SDL_GetError());
    
    Uint32 rmask = 0;
    Uint32 gmask = 0;
    Uint32 bmask = 0;
    Uint32 amask = 0;
    int bpp = 32;

    if(!SDL_PixelFormatEnumToMasks(format, &bpp, &rmask, &gmask, &bmask, &amask))
        throw std::runtime_error(SDL_GetError());
    
    Surface dst = SDL_CreateRGBSurfaceFrom(
        nativePixels, screenWidth, screenHeight, bpp, nativePitch,
        rmask, gmask, bmask, amask);
    if(dst.Null())
        throw std::runtime_error(SDL_GetError());

    SDL_SetSurfaceBlendMode(atlas, SDL_BLENDMODE_BLEND);
    
    //print_pixel_format(dst->format);

    SDL_SetSurfaceColorMod(atlas, 255, 1, 1);
    SDL_FillRect(dst, NULL, 0x00000000);

    if(HasPalette(atlas)) {
        if(SDL_SetSurfacePalette(atlas, palettes.at(paletteIndex))) {
            throw std::runtime_error(SDL_GetError());
        }
    }

    update_surface(dst);
}

void viewer_t::update_surface(Surface dst)
{
    draw_entry(dst, 0, 0, collection, imageIndex);
    // gm1::ImageHeader header = collection.headers.at(imageIndex);
    
    // int anchorX = collection.header.anchorX;
    // int anchorY = collection.header.anchorY;
    // int width = header.width;
    // int height = header.height;
    
    // int drawable = 0;
    // int yoff = anchorY + header.tileY;

    // srand(imageIndex);
    
    // for(int y = yoff; y < screenHeight - height; y += TILE_RHOMBUS_HEIGHT / 2) {
    //     for(int x = anchorX; x < screenWidth - width; x += TILE_RHOMBUS_WIDTH) {
    //         int xoff = 0;
    //         if((y - yoff) % TILE_RHOMBUS_HEIGHT == 0)
    //             xoff = TILE_RHOMBUS_WIDTH / 2;
    //         draw_entry(dst, x + xoff, y, collection, rand() % collection.header.imageCount);
    //         ++drawable;
    //     }
    // }
    
    // SDL_Log("Drawable: %d", drawable);
}

void viewer_t::draw_entry(Surface dst, int x, int y, const gm1::Collection &gm1, size_t index)
{
    gm1::ImageHeader header = collection.headers.at(index);
    
    SDL_Rect srcrect;
    srcrect.x = header.posX;
    srcrect.y = header.posY;
    srcrect.w = header.width;
    srcrect.h = header.height;
    
    SDL_Rect dstrect;
    dstrect.x = x;
    dstrect.y = y;
    dstrect.w = header.width;
    dstrect.h = header.height;

    // SDL_SetSurfaceColorMod(atlas, 255, 0, 0);
    
    BlitSurface(atlas, &srcrect, dst, NULL);
    
    // gm1::ImageHeader header = gm1.headers.at(index);
    // Uint32 size = gm1.sizes.at(index);
    // Uint32 offset = gm1.offsets.at(index);

    // scoped_rwops src(SDL_RWFromConstMem(data.to_uint8(), data.Size()));
    // if(!src)
    //     throw std::runtime_error("SDL_RWFromConstMem failed");
    // if(offset != SDL_RWseek(src.get(), offset, RW_SEEK_SET))
    //     throw std::runtime_error("unexpected offset");
    
    // switch(gm1.encoding()) {
    // case gm1::Encoding::TGX16:
    //     {
    //         SDL_Rect dstrect;
    //         dstrect.x = x - gm1.header.anchorX;
    //         dstrect.y = y - gm1.header.anchorY;;
    //         dstrect.w = header.width;
    //         dstrect.h = header.height;

    //         Surface ref = SubSurface(dst, &dstrect);
    //         tgx::DecodeTGX(src.get(), size, ref);
    //     }
    //     break;
        
    // case gm1::Encoding::Bitmap:
    //     {
    //         SDL_Rect dstrect;
    //         dstrect.x = x - gm1.header.anchorX;
    //         dstrect.y = y - gm1.header.anchorY;
    //         dstrect.w = header.width;
    //         dstrect.h = header.height - 7;

    //         Surface ref = SubSurface(dst, &dstrect);
    //         tgx::DecodeUncompressed(src.get(), size, ref);
    //     }
    //     break;

    // case gm1::Encoding::TileObject:
    //     {
    //         SDL_Rect tilerect;
    //         tilerect.x = x - gm1.header.anchorX;
    //         tilerect.y = y - gm1.header.anchorY;
    //         tilerect.w = TILE_RHOMBUS_WIDTH;
    //         tilerect.h = TILE_RHOMBUS_HEIGHT;

    //         Surface tile = SubSurface(dst, &tilerect);
    //         tgx::DecodeTile(src.get(), TILE_BYTES, tile);

    //         SDL_Rect boxrect;
    //         boxrect.x = x - gm1.header.anchorX + header.hOffset;
    //         boxrect.y = y - gm1.header.anchorY - header.tileY;
    //         boxrect.w = header.boxWidth;
    //         boxrect.h = header.height + TILE_RHOMBUS_HEIGHT;

    //         Surface box = SubSurface(dst, &boxrect);
    //         tgx::DecodeTGX(src.get(), size - TILE_BYTES, box);
    //     }
    //     break;

    // case gm1::Encoding::TGX8:
    //     {
    //         SDL_Rect dstrect;
    //         dstrect.x = x - gm1.header.anchorX;
    //         dstrect.y = y - gm1.header.anchorY;
    //         dstrect.w = header.width;
    //         dstrect.h = header.height;

    //         Surface ref = SubSurface(dst, &dstrect);
    //         tgx::DecodeTGXWithPalette(src.get(), size, ref, palettes.at(paletteIndex));
    //     }
    //     break;

    // default:
    //     throw std::runtime_error("Unknown encoding");
    // }
}

int explorer_main(const std::string &filename)
{
    try {
        scoped_rwops src(SDL_RWFromFile(filename.c_str(), "rb"));
        
        if(!src)
            throw std::runtime_error(SDL_GetError());
        
        gm1::Collection collection(src.get());
        FileBuffer rest(src.get());
        viewer_t viewer(collection, rest);
        viewer.enter_event_loop();
        return 0;
        
    } catch(const std::exception &e) {
        SDL_Log("Exception: %s", e.what());
        return -1;
    }
}

int main(int argc, const char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    if(argc > 1) {
        std::string filename = argv[1];
        return explorer_main(filename);
    }
    SDL_Quit();
    return 0;
}
