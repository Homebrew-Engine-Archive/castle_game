#include "renderer.h"

#include <map>
#include <vector>
#include <functional>

#include <boost/current_function.hpp>
#include <boost/algorithm/clamp.hpp>

#include <game/gameexception.h>
#include <game/make_unique.h>
#include <game/text.h>
#include <game/textrenderer.h>
#include <game/font.h>
#include <game/sdl_utils.h>
#include <game/collection.h>

namespace
{    
    const int MinOutputWidth = 320;
    const int MinOutputHeight = 240;
    
    // TODO sync with driver's texture max width and height
    const int MaxOutputWidth = 4096;
    const int MaxOutputHeight = 4096;

    int AdjustWidth(int width)
    {
        return boost::algorithm::clamp(width, MinOutputWidth, MaxOutputWidth);
    }

    int AdjustHeight(int height)
    {
        return boost::algorithm::clamp(height, MinOutputHeight, MaxOutputHeight);
    }
    
}

namespace Render
{
    Renderer::Renderer() = delete;
    Renderer::Renderer(const Renderer &) = delete;
    Renderer::Renderer(Renderer &&) = default;
    Renderer& Renderer::operator=(const Renderer &) = delete;
    Renderer& Renderer::operator=(Renderer &&) = default;
    Renderer::~Renderer() = default;
    
    Renderer::Renderer(SDL_Renderer *renderer)
        : mRenderer(renderer)
        , mTextRenderer(std::move(CreateTextRenderer(renderer)))
        , mScreenWidth(0)
        , mScreenHeight(0)
        , mScreenFormat(0)
        , mScreenTexture(nullptr)
        , mScreenSurface(nullptr)
        , mTextOverlay()
        , mGFXCache()
        , mGMCache()
    {
        SDL_Rect rect = GetOutputSize();
        mScreenWidth = rect.w;
        mScreenHeight = rect.h;

        std::clog << "GetOutputSize(): " << rect << std::endl;
    }

    bool Renderer::CreateFrameTexture(int width, int height)
    {
        // NOTE
        // Width and height wan't be checked for min and max constraints.
        // It's assumed that they already being checked.
        if((width == 0) || (height == 0)) {
            std::cerr << "Size is zero. Abort allocation."
                      << std::endl;
            return false;
        }
    
        if((mScreenTexture) && (width == mScreenWidth) && (height == mScreenHeight)) {
            std::cerr << "Size of texture matches. Skip allocation."
                      << std::endl;
            return true;
        }
    
        mScreenWidth = width;
        mScreenHeight = height;
        mScreenFormat = SDL_PIXELFORMAT_ARGB8888;
        mScreenTexture =
            TexturePtr(
                SDL_CreateTexture(
                    mRenderer,
                    mScreenFormat,
                    SDL_TEXTUREACCESS_STREAMING,
                    mScreenWidth,
                    mScreenHeight));

        if(!mScreenTexture) {
            throw SDLException(BOOST_CURRENT_FUNCTION, __FILE__, __LINE__);
        }
    
        return true;
    }

    bool Renderer::CreateFrameSurface(void *pixels, int width, int height, int pitch)
    {
        Surface surface = CreateSurfaceFrom(pixels, width, height, pitch, mScreenFormat);
        if(!surface) {
            throw SDLException(BOOST_CURRENT_FUNCTION, __FILE__, __LINE__);
        }

        mScreenSurface = surface;
        return true;
    }

    bool Renderer::ReallocationRequired(int width, int height)
    {
        return (width != mScreenWidth) || (height != mScreenHeight);
    }
    
    Surface Renderer::BeginFrame()
    {
        if(!mTextOverlay.empty()) {
            std::clog << "Discard text overlay which has "
                      << std::dec << mTextOverlay.size()
                      << std::endl;
            mTextOverlay.clear();
        }

        if(!mScreenSurface.Null()) {
            std::cerr << "Previously allocated surface is not null"
                      << std::endl;
            mScreenSurface.reset();
        }
    
        if(!mScreenTexture) {
            if(!CreateFrameTexture(mScreenWidth, mScreenHeight)) {
                std::cerr << "Can't allocate frame texture"
                          << std::endl;
                return Surface();
            }
        }
    
        int nativePitch = 0;
        void *nativePixels = nullptr;
        if(SDL_LockTexture(mScreenTexture.get(), nullptr, &nativePixels, &nativePitch)) {
            std::cerr << "SDL_LockTexture failed: "
                      << SDL_GetError()
                      << std::endl;
            return Surface();
        }
        
        if(!CreateFrameSurface(nativePixels, mScreenWidth, mScreenHeight, nativePitch)) {
            std::cerr << "Can't allocate framebuffer"
                      << std::endl;
            SDL_UnlockTexture(mScreenTexture.get());
            return Surface();
        }

        return mScreenSurface;
    }

    void Renderer::EndFrame()
    {
        if(!mScreenSurface.Null()) {
            SDL_RenderClear(mRenderer);

            /// \note Pixel are not deallocated only surface
            mScreenSurface.reset();
        
            SDL_UnlockTexture(mScreenTexture.get());

            const SDL_Rect textureRect = MakeRect(mScreenWidth, mScreenHeight);
            if(SDL_RenderCopy(mRenderer, mScreenTexture.get(), &textureRect, &textureRect) < 0) {
                std::cerr << "SDL_RenderCopy failed: "
                          << SDL_GetError()
                          << std::endl;
            }
        }

        if(!mTextOverlay.empty()) {
            for(TextBatch batch : mTextOverlay) {
                batch();
            }
            mTextOverlay.clear();
        }
    
        SDL_RenderPresent(mRenderer);
    }

    SDL_Rect Renderer::GetOutputSize() const
    {
        int width = 0;
        int height = 0;
        if(SDL_GetRendererOutputSize(mRenderer, &width, &height)) {
            std::cerr << "SDL_GetRendererOutputSize failed: "
                      << SDL_GetError()
                      << std::endl;
        }
        return MakeRect(width, height);
    }

    void Renderer::SetWindowSize(int, int)
    {
    }
    
    void Renderer::AdjustBufferSize(int width, int height)
    {
        if(!mScreenSurface.Null()) {
            throw std::runtime_error("AdjustBufferSize called with active frame.");
        }

        // Cutting up and down texture height and width
        int adjustedWidth = AdjustWidth(width);
        int adjustedHeight = AdjustHeight(height);

        // NOTE
        // This is the only place we limit width and height
        if(ReallocationRequired(adjustedWidth, adjustedHeight)) {
            std::clog << "AdjustBufferSize(): " << std::dec
                      << MakeRect(adjustedWidth, adjustedHeight)
                      << std::endl;
            CreateFrameTexture(adjustedWidth, adjustedHeight);
        }
    }

    Surface Renderer::QuerySurface(const FilePath &filename)
    {
        auto cached = mGFXCache.find(filename);
        if(cached != mGFXCache.end()) {
            return cached->second;
        } else {
            Surface loaded = LoadSurface(filename);
            mGFXCache.insert({filename, loaded});
            return loaded;
        }
    }

    const CollectionData &Renderer::QueryCollection(const FilePath &filename)
    {
        auto searchResult = mGMCache.find(filename);
        if(searchResult != mGMCache.end()) {
            return *searchResult->second;
        } else {
            CollectionDataPtr ptr =
                std::move(
                    LoadCollectionData(filename));

            if(!ptr)
                throw std::runtime_error("Unable to load collection");
        
            const CollectionData &data = *ptr;
            mGMCache.insert(
                std::make_pair(filename, std::move(ptr)));

            return data;
        }
    }

    bool Renderer::CacheCollection(const FilePath &filename)
    {
        try {
            QueryCollection(filename);
            return true;
        } catch(const std::exception &error) {
            std::cerr << "Cache collection failed: " << error.what()
                      << std::endl;
            return false;
        }
    }

    bool Renderer::CacheFontCollection(const FontCollectionInfo &info)
    {
        try {
            CollectionDataPtr data =
                std::move(
                    LoadCollectionData(info.filename));
            if(!data)
                throw std::runtime_error("Unable load font collection");
        
            size_t skip = 0;
            for(int fontSize : info.sizes) {
                Render::Font font = Render::MakeFont(*data, info.alphabet, skip);
                if(!mTextRenderer->CacheFont(info.name, fontSize, font)) {
                    std::cerr << "Unable to cache font: "
                              << info.name << ' '
                              << fontSize << std::endl;
                }
                skip += info.alphabet.size();
            }
        
            return true;
        } catch(const std::exception &e) {
            std::cerr << "In CacheFontCollection: " << std::endl
                      << e.what() << std::endl;
            return false;
        }
    }

    void Renderer::SetFont(const std::string &fontname, int size)
    {
        auto changeFont = [fontname, size, this]() {
            mTextRenderer->SetFont(fontname, size);
        };
        mTextOverlay.push_back(changeFont);
    }

    void Renderer::SetColor(const SDL_Color &color)
    {
        auto changeColor = [color, this]() {
            mTextRenderer->SetColor(color);
        };
        mTextOverlay.push_back(changeColor);
    }

    void Renderer::RenderTextLine(const std::string &text, const SDL_Point &point)
    {
        auto drawText = [text, point, this]() {
            SDL_Rect textRect = mTextRenderer->CalculateTextRect(text);
    
            mTextRenderer->SetCursor(
                ShiftPoint(point, 0, textRect.h));
    
            mTextRenderer->PutString(text);
        };
    
        mTextOverlay.push_back(drawText);
    }

    void Renderer::RenderTextBox(const std::string &text, const SDL_Rect &rect,
                                     AlignH alignh, AlignV alignv)
    {
        RenderTextLine(text, TopLeft(rect));
    }

} // namespace Render
