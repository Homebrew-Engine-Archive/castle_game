#include "renderer.h"

Renderer::Renderer(Window &window)
    : fbTexture(NULL)
    , fbWidth(0)
    , fbHeight(0)
    , fbFormat(0)
{
    SDL_Window *sdl_wnd = window.GetWindow();
    renderer = SDL_CreateRenderer(sdl_wnd, -1, 0);
    if(renderer == NULL)
        throw std::runtime_error(SDL_GetError());
    SDL_GetWindowSize(sdl_wnd, &fbWidth, &fbHeight);
    AdjustOutputSize(fbWidth, fbHeight);
}

Renderer::~Renderer()
{
    fbSurface.reset();
    SDL_DestroyTexture(fbTexture);
    SDL_DestroyRenderer(renderer);
}

bool Renderer::AllocFrameTexture(int width, int height)
{
    std::clog << "Allocating frame texture: " << std::endl
              << std::dec
              << "\tWidth = " << width << std::endl
              << "\tHeight = " << height << std::endl;
    
    if((width == 0) || (height == 0)) {
        std::cerr << "Size is zero. Abort allocation."
                  << std::endl;
        return false;
    }
    
    if((fbTexture != NULL) && (width == fbWidth) && (height == fbHeight)) {
        std::cerr << "Size of texture matches. Skip allocation."
                  << std::endl;
        return true;
    }
    
    if(fbTexture != NULL) {
        SDL_DestroyTexture(fbTexture);
    }
    
    fbWidth = width;
    fbHeight = height;
    fbFormat = SDL_PIXELFORMAT_ARGB8888;
    fbTexture = SDL_CreateTexture(
        renderer,
        fbFormat,
        SDL_TEXTUREACCESS_STREAMING,
        fbWidth,
        fbHeight);
    
    if(fbTexture == NULL) {
        std::cerr << "SDL_CreateTexture failed: "
                  << SDL_GetError()
                  << std::endl;
        return false;
    }

    return true;
}

Surface Renderer::BeginFrame()
{
    if(fbTexture == NULL) {
        if(!AllocFrameTexture(fbWidth, fbHeight)) {
            std::cerr << "Can't allocate frame texture"
                      << std::endl;
            return Surface();
        }
    }
    
    if(fbSurface.Null()) {
        int nativePitch;
        void *nativePixels;
        if(SDL_LockTexture(fbTexture, NULL, &nativePixels, &nativePitch)) {
            std::cerr << "SDL_LockTexture failed: "
                      << SDL_GetError()
                      << std::endl;
            return Surface();
        }
        
        fbSurface = AllocFrameSurface(
            nativePixels, fbWidth, fbHeight, nativePitch);
        if(fbSurface.Null()) {
            std::cerr << "Can't allocate framebuffer"
                      << std::endl;
            SDL_UnlockTexture(fbTexture);
            return Surface();
        }
    }

    return fbSurface;
}

void Renderer::EndFrame()
{
    if(!fbSurface.Null()) {
        SDL_RenderClear(renderer);
        
        fbSurface.reset();
        SDL_UnlockTexture(fbTexture);
        if(SDL_RenderCopy(renderer, fbTexture, NULL, NULL)) {
            std::cerr << "SDL_RenderCopy failed: "
                      << SDL_GetError()
                      << std::endl;
        }
        SDL_RenderPresent(renderer);
    }
}

Surface Renderer::AllocFrameSurface(void *pixels, int width, int height, int pitch)
{
    Uint32 rmask;
    Uint32 gmask;
    Uint32 bmask;
    Uint32 amask;
    int bpp;

    if(!SDL_PixelFormatEnumToMasks(fbFormat, &bpp, &rmask, &gmask, &bmask, &amask)) {
        std::cerr << "SDL_PixelFormatEnumToMasks failed: "
                  << SDL_GetError()
                  << std::endl;
        return Surface();
    }

    Surface surface = SDL_CreateRGBSurfaceFrom(pixels, width, height, bpp, pitch, rmask, gmask, bmask, amask);
    if(surface.Null()) {
        std::cerr << "SDL_CreateRGBSurfaceFrom failed: "
                  << SDL_GetError()
                  << std::endl;
        return Surface();
    }
    
    return surface;
}

SDL_Rect Renderer::GetOutputSize() const
{
    int width;
    int height;
    if(SDL_GetRendererOutputSize(renderer, &width, &height)) {
        std::cerr << "SDL_GetRendererOutputSize failed: "
                  << SDL_GetError()
                  << std::endl;
    }
    return MakeRect(width, height);
}

void Renderer::AdjustOutputSize(int width, int height)
{
    if(fbSurface.Null()) {
        AllocFrameTexture(width, height);
    } else {
        SDL_Rect rect = SurfaceBounds(fbSurface);
        if(rect.w != width || rect.h != height) {
            EndFrame();
            AllocFrameTexture(width, height);
            fbSurface = BeginFrame();
        }
    }
}

Surface Renderer::LoadSurface(const std::string &filename)
{
    std::clog << "Load surface from: " << filename;
    std::clog << std::endl;
    try {
        FileBuffer filebuff(filename.c_str(), "rb");
        scoped_rwops src = std::move(RWFromFileBuffer(filebuff));
        
        if(!src)
            throw std::runtime_error("Can't alloc file buff");
        
        return tgx::LoadStandaloneImage(src.get());
        
    } catch(const std::exception &e) {
        std::cerr << "Exception in LoadImage: " << std::endl
                  << "\tFilename: " << filename.c_str() << std::endl
                  << "\tWhat: " << e.what() << std::endl;
        return Surface();
    }
}

Surface Renderer::QuerySurface(const std::string &filename)
{
    auto cached = m_imageCache.find(filename);
    if(cached != m_imageCache.end()) {
        return cached->second;
    } else {
        Surface loaded = LoadSurface(filename);
        m_imageCache.insert(std::make_pair(filename, loaded));
        return loaded;
    }
}

bool Renderer::LoadImageCollection(const std::string &filename)
{
    std::clog << "Load image collection: "
              << filename
              << std::endl;
    try {
        FileBuffer filebuff(filename.c_str(), "rb");
        scoped_rwops src = std::move(RWFromFileBuffer(filebuff));
        if(!src)
            throw std::runtime_error("file not readable");
        gm1::Collection gm1(src.get());
        std::vector<Surface> atlas;
        gm1::LoadEntries(src.get(), gm1, atlas);
        CollectionData collection;
        collection.header = gm1.header;
        for(size_t n = 0; n < GM1_PALETTE_COUNT; ++n) {
            SDL_Palette *palette =
                gm1::CreateSDLPaletteFrom(gm1.palettes[n]);
            collection.palettes.emplace_back(palette);
        }
        for(size_t n = 0; n < gm1.header.imageCount; ++n) {
            gm1::ImageHeader header = gm1.headers[n];
            Surface surface = atlas[n];
            collection.entries.emplace_back(header, surface);
        }
        m_cache.insert(std::make_pair(filename, collection));
        return true;
        
    } catch(const std::exception &e) {
        std::cerr << "Exception in LoadImageCollection: " << std::endl
                  << "\tFilename: " << filename << std::endl
                  << "\tWhat: " << e.what() << std::endl;
        return false;
    }
}

CollectionData Renderer::GetCollection(const std::string &filename) const
{
    auto searchResult = m_cache.find(filename);
    if(searchResult == m_cache.end())
        throw std::runtime_error("There are no such cache entry. Go out!");
    return searchResult->second;
}

static void PrintDriverInfo(const SDL_RendererInfo &info)
{           
    std::clog << "\tname: "
              << info.name
              << std::endl;
            
    std::clog << "\tnum_texture_formats: "
              << std::dec
              << info.num_texture_formats
              << std::endl;
            
    std::clog << "\tmax_texture_width: "
              << std::dec
              << info.max_texture_width
              << std::endl;
            
    std::clog << "\tmax_texture_height: "
              << std::dec
              << info.max_texture_height
              << std::endl;

    std::clog << "\ttexture_formats: ";
    if(info.num_texture_formats == 0) {
        std::clog << "None";
    } else {
        for(size_t index = 0; index < info.num_texture_formats; ++index) {
            std::clog << std::hex
                      << info.texture_formats[index]
                      << " ";
        }
    }
    std::clog << std::endl;

#define PRINT_FLAG_IF_ANY(flags, flag)          \
    if(flags & flag)                            \
        std::clog << #flag << " | ";
            
    std::clog << "\tflags: ";
    if(info.flags != 0) {
        PRINT_FLAG_IF_ANY(info.flags, SDL_RENDERER_SOFTWARE);
        PRINT_FLAG_IF_ANY(info.flags, SDL_RENDERER_ACCELERATED);
        PRINT_FLAG_IF_ANY(info.flags, SDL_RENDERER_PRESENTVSYNC);
        PRINT_FLAG_IF_ANY(info.flags, SDL_RENDERER_TARGETTEXTURE);
    } else {
        std::clog << "None";
    }
    std::clog << std::endl;
#undef PRINT_FLAG_IF_ANY
}

void EnumRenderDrivers()
{
    int num = SDL_GetNumRenderDrivers();

    std::clog << "Drivers avialable: ";
    std::clog << std::dec << num;
    std::clog << std::endl;

    for(int index = 0; index < num; ++index) {
        SDL_RendererInfo info;
        std::clog << "Driver with index: ";
        std::clog << std::dec;
        std::clog << index;
        std::clog << std::endl;
        if(SDL_GetRenderDriverInfo(index, &info)) {
            std::clog << "Can't query driver info";
            std::clog << std::endl;
        } else {
            PrintDriverInfo(info);
        }
    }
}

void PrintRendererInfo(SDL_Renderer *renderer)
{
    if(renderer == NULL) {
        return;
    }
    std::clog << "Renderer info: " << std::endl;
    SDL_RendererInfo info;
    if(SDL_GetRendererInfo(renderer, &info)) {
        std::clog << "\tCan't query renderer info" << std::endl;
    } else {
        PrintDriverInfo(info);
    }
}
