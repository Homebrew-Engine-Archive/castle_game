#include "renderer.h"

#include <map>
#include <vector>
#include <functional>

#include <boost/current_function.hpp>
#include <boost/algorithm/clamp.hpp>

#include <game/gameexception.h>
#include <game/make_unique.h>
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
    Renderer::Renderer(SDL_Renderer *renderer)
        : mRenderer(renderer)
        , mScreenWidth(0)
        , mScreenHeight(0)
        , mScreenFormat(0)
        , mScreenClear(true)
        , mScreenTexture(nullptr)
        , mScreenSurface(nullptr)
        , mGFXCache()
        , mGMCache()
    {
        SDL_Rect rect = GetOutputSize();
        mScreenWidth = rect.w;
        mScreenHeight = rect.h;
    }

    void Renderer::CreateScreenTexture(int width, int height)
    {
        if((width == 0) || (height == 0)) {
            return;
        }
    
        if((mScreenTexture) && (width == mScreenWidth) && (height == mScreenHeight)) {
            return;
        }
    
        mScreenWidth = width;
        mScreenHeight = height;
        mScreenFormat = SDL_PIXELFORMAT_ARGB8888;
        mScreenTexture.reset(
            SDL_CreateTexture(
                mRenderer,
                mScreenFormat,
                SDL_TEXTUREACCESS_STREAMING,
                mScreenWidth,
                mScreenHeight));

        if(!mScreenTexture) {
            throw SDLException(BOOST_CURRENT_FUNCTION, __FILE__, __LINE__);
        }
    }

    void Renderer::CreateScreenSurface(void *pixels, int pitch)
    {
        Surface surface = CreateSurfaceFrom(pixels, mScreenWidth, mScreenHeight, pitch, mScreenFormat);
        if(!surface) {
            throw SDLException(BOOST_CURRENT_FUNCTION, __FILE__, __LINE__);
        }
        mScreenSurface = surface;
    }

    bool Renderer::ReallocationRequired(int width, int height)
    {
        return (width != mScreenWidth) || (height != mScreenHeight);
    }
    
    Surface Renderer::BeginFrame()
    {
        if(!mScreenSurface.Null()) {
            mScreenSurface.reset();
        }
    
        if(!mScreenTexture) {
            CreateScreenTexture(mScreenWidth, mScreenHeight);
        }
        
        mLock.reset(new TextureLocker(mScreenTexture.get()));
        CreateScreenSurface(mLock->Pixels(), mLock->Pitch());
        return mScreenSurface;
    }

    void Renderer::EndFrame()
    {
        if(!mScreenSurface.Null()) {
            if(mScreenClear) {
                SDL_RenderClear(mRenderer);
            }

            mScreenSurface.reset(nullptr);
            mLock.reset(nullptr);

            const SDL_Rect textureRect = MakeRect(mScreenWidth, mScreenHeight);
            if(SDL_RenderCopy(mRenderer, mScreenTexture.get(), &textureRect, &textureRect) < 0) {
                throw SDLException(BOOST_CURRENT_FUNCTION, __FILE__, __LINE__);
            }
        }
    
        SDL_RenderPresent(mRenderer);
    }

    SDL_Rect Renderer::GetOutputSize() const
    {
        SDL_Rect outputSize { 0, 0, 0, 0 };
        if(SDL_GetRendererOutputSize(mRenderer, &outputSize.w, &outputSize.h) < 0) {
            throw SDLException(BOOST_CURRENT_FUNCTION, __FILE__, __LINE__);
        }
        
        return outputSize;
    }

    void Renderer::SetWindowSize(int width, int height)
    {
        AdjustBufferSize(width, height);
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
            CreateScreenTexture(adjustedWidth, adjustedHeight);
        }
    }

    Surface Renderer::QuerySurface(const fs::path &filename)
    {
        auto cached = mGFXCache.find(filename);
        if(cached != mGFXCache.end()) {
            return cached->second;
        }
        
        Surface loaded = LoadSurface(filename);
        mGFXCache.insert({filename, loaded});
        return loaded;
    }

    CollectionData const& Renderer::QueryCollection(const fs::path &filename)
    {
        auto searchResult = mGMCache.find(filename);
        if(searchResult != mGMCache.end()) {
            return *searchResult->second;
        }
        
        CollectionDataPtr &&ptr = LoadCollectionData(filename);

        if(!ptr) {
            throw std::runtime_error("Unable to load collection");
        }
        
        const CollectionData &data = *ptr;
        mGMCache.insert(
            std::make_pair(filename, std::move(ptr)));

        return data;
    }

    bool Renderer::CacheCollection(const fs::path &filename)
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

    void Renderer::EnableClearScreen(bool on)
    {
        mScreenClear = on;
    }
} // namespace Render
