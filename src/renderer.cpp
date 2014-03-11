#include "renderer.h"

CollectionAtlas::CollectionAtlas(SDL_RWops *src)
    : gm1(src)
    , map(gm1::LoadAtlas(src, gm1))
{ }

CollectionEntry::CollectionEntry(const gm1::ImageHeader &hdr_, const Surface &sf_)
    : header(hdr_)
    , surface(sf_)
{ }

Renderer::Renderer(Window *window)
    : m_fbWidth(0)
    , m_fbHeight(0)
    , m_fbFormat(0)
{
    SDL_Window *sdl_wnd = window->GetWindow();
    m_renderer =
        RendererPtr(
            SDL_CreateRenderer(sdl_wnd, -1, 0));
    if(!m_renderer)
        throw std::runtime_error(SDL_GetError());
    m_textRenderer =
        TextRendererPtr(
            new TextRenderer(m_renderer.get()));
    SDL_GetWindowSize(sdl_wnd, &m_fbWidth, &m_fbHeight);
    AdjustOutputSize(m_fbWidth, m_fbHeight);
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
    
    if((m_fbTexture) && (width == m_fbWidth) && (height == m_fbHeight)) {
        std::cerr << "Size of texture matches. Skip allocation."
                  << std::endl;
        return true;
    }
    
    m_fbWidth = width;
    m_fbHeight = height;
    m_fbFormat = SDL_PIXELFORMAT_ARGB8888;
    m_fbTexture =
        TexturePtr(
            SDL_CreateTexture(
                m_renderer.get(),
                m_fbFormat,
                SDL_TEXTUREACCESS_STREAMING,
                m_fbWidth,
                m_fbHeight));

    if(!m_fbTexture) {
        std::cerr << "SDL_CreateTexture failed: "
                  << SDL_GetError()
                  << std::endl;
        return false;
    }

    return true;
}

Surface Renderer::BeginFrame()
{
    if(!m_textOverlay.empty()) {
        std::clog << "Discard text overlay"
                  << std::endl;
        m_textOverlay.clear();
    }
    
    if(!m_fbTexture) {
        if(!AllocFrameTexture(m_fbWidth, m_fbHeight)) {
            std::cerr << "Can't allocate frame texture"
                      << std::endl;
            return Surface();
        }
    }
    
    if(!m_fbSurface.Null()) {
        std::clog << "Nested BeginFrame" << std::endl;
        return m_fbSurface;
    }
    
    int nativePitch;
    void *nativePixels;
    if(SDL_LockTexture(m_fbTexture.get(), NULL, &nativePixels, &nativePitch)) {
        std::cerr << "SDL_LockTexture failed: "
                  << SDL_GetError()
                  << std::endl;
        return Surface();
    }
        
    m_fbSurface = AllocFrameSurface(nativePixels, m_fbWidth, m_fbHeight, nativePitch);
    if(m_fbSurface.Null()) {
        std::cerr << "Can't allocate framebuffer"
                  << std::endl;
        SDL_UnlockTexture(m_fbTexture.get());
        return Surface();
    }

    return m_fbSurface;
}

void Renderer::EndFrame()
{
    SDL_Renderer *renderer = GetRenderer();
    if(!m_fbSurface.Null()) {
        SDL_RenderClear(renderer);
        
        m_fbSurface.reset();
        SDL_UnlockTexture(m_fbTexture.get());
        if(SDL_RenderCopy(renderer, m_fbTexture.get(), NULL, NULL)) {
            std::cerr << "SDL_RenderCopy failed: "
                      << SDL_GetError()
                      << std::endl;
        }
    }

    if(!m_textOverlay.empty()) {
        for(const auto &text : m_textOverlay) {
            RenderTextOverlay(text);
        }
        m_textOverlay.clear();
    }
    
    SDL_RenderPresent(renderer);
}

Surface Renderer::AllocFrameSurface(void *pixels, int width, int height, int pitch)
{
    Uint32 rmask;
    Uint32 gmask;
    Uint32 bmask;
    Uint32 amask;
    int bpp;
    if(!SDL_PixelFormatEnumToMasks(m_fbFormat, &bpp, &rmask, &gmask, &bmask, &amask)) {
        std::cerr << "SDL_PixelFormatEnumToMasks failed: "
                  << SDL_GetError()
                  << std::endl;
        return Surface();
    }

    Surface surface = SDL_CreateRGBSurfaceFrom(
        pixels, width, height, bpp, pitch, rmask, gmask, bmask, amask);
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
    if(SDL_GetRendererOutputSize(m_renderer.get(), &width, &height)) {
        std::cerr << "SDL_GetRendererOutputSize failed: "
                  << SDL_GetError()
                  << std::endl;
    }
    return MakeRect(width, height);
}

bool Renderer::ReallocationRequired(int width, int height)
{
    const int threshold = 100;
    return ((width / threshold) != (m_fbWidth / threshold))
        || ((height / threshold) != (m_fbHeight / threshold));
}

void Renderer::AdjustOutputSize(int width, int height)
{
    if(!m_fbSurface.Null()) {
        throw std::runtime_error("AdjustOutputSize called with active frame.");
    }
    if(ReallocationRequired(width, height)) {
        AllocFrameTexture(width, height);
    }
}

Surface Renderer::LoadSurface(const std::string &filename) const
{
    std::clog << "Load surface from: " << filename;
    std::clog << std::endl;
    try {
        FileBuffer filebuff(filename.c_str(), "rb");
        RWPtr src = std::move(RWFromFileBuffer(filebuff));
        
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

CollectionDataPtr Renderer::LoadCollection(const std::string &filename) const
{
    std::clog << "Load collection: "
              << filename
              << std::endl;
    
    try {
        FileBuffer filebuff(filename, "rb");
        RWPtr src = std::move(RWFromFileBuffer(filebuff));
        
        if(!src)
            throw std::runtime_error("file not readable");
        
        gm1::Collection gm1(src.get());

        CollectionDataPtr ptr(new CollectionData);
        ptr->header = gm1.header;
        
        std::vector<Surface> atlas;
        gm1::LoadEntries(src.get(), gm1, atlas);
        
        for(size_t n = 0; n < GM1_PALETTE_COUNT; ++n) {
            PalettePtr palette =
                PalettePtr(
                    gm1::CreateSDLPaletteFrom(gm1.palettes[n]));
            ptr->palettes.push_back(std::move(palette));
        }
        
        for(size_t n = 0; n < gm1.header.imageCount; ++n) {
            gm1::ImageHeader header = gm1.headers[n];
            Surface surface = atlas[n];
            ptr->entries.emplace_back(header, surface);
        }

        return ptr;
        
    } catch(const std::exception &e) {
        std::cerr << "Exception in LoadImageCollection: " << std::endl
                  << "\tFilename: " << filename << std::endl
                  << "\tWhat: " << e.what() << std::endl;
        return CollectionDataPtr(nullptr);
    }
}

CollectionAtlasPtr Renderer::LoadCollectionAtlas(const std::string &filename) const
{
    std::clog << "Load collection atlas: " << filename << std::endl;

    try {
        FileBuffer filebuff(filename, "rb");
        RWPtr src = std::move(RWFromFileBuffer(filebuff));
        if(!src)
            throw std::runtime_error("file not readable");
        
        CollectionAtlasPtr ptr(
            new CollectionAtlas(src.get()));

        return ptr;
        
    } catch(const std::exception &e) {
        std::cerr << "Exception in LoadCollectionAtlas: "
                  << e.what()
                  << std::endl;
        return CollectionAtlasPtr(nullptr);
    }
}

const CollectionData &Renderer::QueryCollection(const std::string &filename)
{
    auto searchResult = m_cache.find(filename);
    if(searchResult != m_cache.end()) {
        return *searchResult->second;
    } else {
        CollectionDataPtr ptr =
            std::move(
                LoadCollection(filename));

        if(!ptr)
            throw std::runtime_error("Unable to load collection");
        
        const CollectionData &data = *ptr;
        m_cache.insert(
            std::make_pair(filename, std::move(ptr)));

        return data;
    }
}

bool Renderer::CacheCollection(const std::string &filename)
{
    try {
        QueryCollection(filename);
        return true;
    } catch(const std::exception &e) {
        std::cerr << "Cache collection failed"
                  << std::endl;
        return false;
    }
}

bool Renderer::CacheFontCollection(const FontCollectionInfo &info)
{
    try {
        CollectionDataPtr data =
            std::move(
                LoadCollection(info.filename));
        if(!data)
            throw std::runtime_error("Unable load font collection");
        
        size_t skip = 0;
        for(font_size_t size : info.sizes) {
            Font font(*data, info.alphabet, skip);
            m_textRenderer->RegisterFont(info.name, size, font);
            skip += info.alphabet.size();
        }
        
        return true;
    } catch(const std::exception &e) {
        std::cerr << "Exception in CacheFontCollection: "
                  << e.what()
                  << std::endl;
        return false;
    }
}

void Renderer::RenderTextOverlay(const TextData &item)
{
    m_textRenderer->SetFont(item.fontname, item.size);
    m_textRenderer->SetColor(item.color);
    
    SDL_Rect textRect = m_textRenderer->CalculateTextRect(item.text);
    SDL_Rect aligned = PutIn(textRect, item.rect, -1.0f, -1.0f);

    m_textRenderer->SetCursor(
        BottomLeft(aligned));
    
    for(char c : item.text) {
        m_textRenderer->PutChar(c);
    }
}

void Renderer::RenderTextLine(
    const std::string &text,
    const SDL_Rect *rect,
    const std::string &fontname,
    const SDL_Color &color,
    font_size_t size)
{
    TextData task;
    task.text = text;
    task.fontname = fontname;
    task.color = color;
    task.size = size;

    if(rect != NULL) {
        task.rect = *rect;
    } else {
        task.rect = GetOutputSize();
    }
    
    m_textOverlay.push_back(task);
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

    std::clog << "\tflags: ";
    if(info.flags != 0) {
        if(info.flags & SDL_RENDERER_SOFTWARE)
            std::clog << "SDL_RENDERER_SOFTWARE" << " | ";
        if(info.flags & SDL_RENDERER_ACCELERATED)
            std::clog << "SDL_RENDERER_ACCELERATED" << " | ";
        if(info.flags & SDL_RENDERER_PRESENTVSYNC)
            std::clog << "SDL_RENDERER_PRESENTVSYNC" << " | ";
        if(info.flags & SDL_RENDERER_TARGETTEXTURE)
            std::clog << "SDL_RENDERER_TARGETTEXTURE" << " | ";
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

SDL_Color MakeColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    return SDL_Color { r, g, b, a };
}
