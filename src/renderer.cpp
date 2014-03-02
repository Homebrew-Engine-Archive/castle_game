#include "renderer.h"

Renderer::Renderer(SDLWindow &window)
    : fbTexture(NULL)
{
    renderer = SDL_CreateRenderer(
        window.GetWindow(), -1,
        0);

    if(renderer == NULL)
        throw std::runtime_error(SDL_GetError());

    int width, height;
    if(SDL_GetRendererOutputSize(renderer, &width, &height)) {
        throw std::runtime_error(SDL_GetError());
    }
    ReallocateStreamingTexture(width, height);
    std::clog << "ReallocateStreamingTexture: ";
    std::clog << "Width=" << width <<',';
    std::clog << "Height=" << height;
    std::clog << std::endl;
}

Renderer::~Renderer()
{
    fbSurface.reset();
    SDL_DestroyTexture(fbTexture);
    SDL_DestroyRenderer(renderer);
}

void Renderer::ReallocateStreamingTexture(int width, int height)
{
    if(fbTexture != NULL) {
        SDL_DestroyTexture(fbTexture);
        fbTexture = NULL;
    }

    fbFormat = SDL_PIXELFORMAT_ARGB8888;
    
    fbTexture = SDL_CreateTexture(
        renderer, fbFormat,
        SDL_TEXTUREACCESS_STREAMING,
        width, height);
    
    if(fbTexture == NULL) {
        std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << std::endl;
    }
}

Surface Renderer::BeginFrame()
{
    if(fbSurface.Null()) {
        int width;
        int height;
        if(SDL_QueryTexture(fbTexture, NULL, NULL, &width, &height)) {
            std::cerr << "SDL_QueryTexture failed: " << SDL_GetError() << std::endl;
        }

        int nativePitch;
        void *nativePixels;
        if(SDL_LockTexture(fbTexture, NULL, &nativePixels, &nativePitch)) {
            std::cerr << "SDL_LockTexture failed: " << SDL_GetError() << std::endl;
            return fbSurface;
        }
        
        fbSurface = AllocateStreamingSurface(nativePixels, width, height, nativePitch);
        if(fbSurface.Null()) {
            std::cerr << "Can't allocate framebuffer" << std::endl;
            SDL_UnlockTexture(fbTexture);
            return fbSurface;
        }
    }
    
    return fbSurface;
}

Surface Renderer::AllocateStreamingSurface(void *pixels, int width, int height, int pitch)
{
    Uint32 rmask;
    Uint32 gmask;
    Uint32 bmask;
    Uint32 amask;
    int bpp;

    if(!SDL_PixelFormatEnumToMasks(fbFormat, &bpp, &rmask, &gmask, &bmask, &amask)) {
        std::cerr << "SDL_PixelFormatEnumToMasks failed: " << std::endl;
        return Surface();
    }

    Surface surface = SDL_CreateRGBSurfaceFrom(pixels, width, height, bpp, pitch, rmask, gmask, bmask, amask);
    if(surface.Null()) {
        std::cerr << "SDL_CreateRGBSurfaceFrom failed: " << SDL_GetError() << std::endl;
    }
    
    return surface;
}

void Renderer::EndFrame()
{
    if(!fbSurface.Null()) {
        fbSurface.reset();
        SDL_UnlockTexture(fbTexture);
    }

    SDL_RenderClear(renderer);
    
    if(SDL_RenderCopy(renderer, fbTexture, NULL, NULL)) {
        std::cerr << "SDL_RenderCopy failed: " << SDL_GetError() << std::endl;
    }

    SDL_RenderPresent(renderer);
}

Surface Renderer::LoadImage(const std::string &filename)
{
    SDL_Log("LoadImage: %s", filename.c_str());
    try {
        FileBuffer filebuff(filename.c_str(), "rb");
        scoped_rwops src = std::move(RWFromFileBuffer(filebuff));
        
        if(!src)
            throw std::runtime_error("Can't alloc file buff");
        
        return tgx::LoadStandaloneImage(src.get());
        
    } catch(const std::exception &e) {
        std::cerr << "Exception in LoadImage: " << std::endl;
        std::cerr << '\t' << filename.c_str() << std::endl;
        std::cerr << '\t' << e.what() << std::endl;
        return Surface();
    }
}

bool Renderer::LoadImageCollection(const std::string &filename)
{
    SDL_Log("LoadImageCollection: %s", filename.c_str());
    try {
        FileBuffer filebuff(filename.c_str(), "rb");
        scoped_rwops src = std::move(RWFromFileBuffer(filebuff));

        if(!src)
            throw std::runtime_error("file not readable");
        
        gm1::Collection gm1(src.get());
        Surface atlas = gm1::LoadAtlas(src.get(), gm1);

        CacheEntry entry = { gm1, atlas };
        m_cache.insert(std::make_pair(filename, entry));
        
        return true;
        
    } catch(const std::exception &e) {
        std::cerr << "Exception in LoadImageCollection: " << std::endl;
        std::cerr << '\t' << filename << std::endl;
        std::cerr << '\t' << e.what() << std::endl;
        return false;
    }
}

static void PrintDriverInfo(const SDL_RendererInfo &info)
{           
    std::clog << "\tname: ";
    std::clog << info.name;
    std::clog << std::endl;
            
    std::clog << "\tnum_texture_formats: ";
    std::clog << std::dec;
    std::clog << info.num_texture_formats;
    std::clog << std::endl;
            
    std::clog << "\tmax_texture_width: ";
    std::clog << std::dec;
    std::clog << info.max_texture_width;
    std::clog << std::endl;
            
    std::clog << "\tmax_texture_height: ";
    std::clog << std::dec;
    std::clog << info.max_texture_height;
    std::clog << std::endl;

    std::clog << "\ttexture_formats: ";
    if(info.num_texture_formats == 0) {
        std::clog << "None";
    } else {
        for(size_t index = 0; index < info.num_texture_formats; ++index) {
            std::clog << std::hex;
            std::clog << info.texture_formats[index];
            std::clog << " ";
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
