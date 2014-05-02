#include "renderer.h"

#include <map>
#include <vector>
#include <functional>

#include <boost/current_function.hpp>
#include <boost/algorithm/clamp.hpp>

#include <game/exception.h>
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
        mScreenFormat = SDL_PIXELFORMAT_ARGB8888;
    }

    void Renderer::CreateScreenTexture(int width, int height)
    {
        if((mScreenTexture) && (width == mScreenWidth) && (height == mScreenHeight)) {
            return;
        }
    
        mScreenTexture.reset(
            SDL_CreateTexture(
                mRenderer,
                mScreenFormat,
                SDL_TEXTUREACCESS_STREAMING,
                width,
                height));

        // Inconsistent size should be reported by SDL itself
        if(!mScreenTexture) {
            throw Castle::SDLException(BOOST_CURRENT_FUNCTION, __FILE__, __LINE__);
        }

        mScreenWidth = width;
        mScreenHeight = height;
    }

    void Renderer::CreateScreenSurface(int width, int height)
    {
        mScreenSurface = CreateSurface(width, height, mScreenFormat);
    }

    bool Renderer::ReallocationRequired(int width, int height)
    {
        return (width != mScreenWidth) || (height != mScreenHeight);
    }
    
    Surface Renderer::BeginFrame()
    {
        if(!mScreenTexture) {
            CreateScreenTexture(mScreenWidth, mScreenHeight);
        }

        if(!mScreenSurface) {
            CreateScreenSurface(mScreenWidth, mScreenHeight);
        }
        
        if(mScreenClear) {
            SDL_FillRect(mScreenSurface, NULL, 0);
        }
        
        return mScreenSurface;
    }

    void Renderer::EndFrame()
    {
        if(!mScreenSurface.Null()) {
            if(SDL_UpdateTexture(mScreenTexture.get(), NULL, mScreenSurface->pixels, mScreenSurface->pitch) < 0) {
                throw Castle::SDLException(BOOST_CURRENT_FUNCTION, __FILE__, __LINE__);
            }

            const SDL_Rect textureRect = MakeRect(mScreenWidth, mScreenHeight);
            if(SDL_RenderCopy(mRenderer, mScreenTexture.get(), &textureRect, &textureRect) < 0) {
                throw Castle::SDLException(BOOST_CURRENT_FUNCTION, __FILE__, __LINE__);
            }
        }

        SDL_RenderPresent(mRenderer);
    }

    SDL_Rect Renderer::GetOutputSize() const
    {
        SDL_Rect outputSize { 0, 0, 0, 0 };
        if(SDL_GetRendererOutputSize(mRenderer, &outputSize.w, &outputSize.h) < 0) {
            throw Castle::SDLException(BOOST_CURRENT_FUNCTION, __FILE__, __LINE__);
        }
        
        return outputSize;
    }

    void Renderer::SetWindowSize(int width, int height)
    {
        AdjustBufferSize(width, height);
    }
    
    void Renderer::AdjustBufferSize(int width, int height)
    {
        mScreenSurface.reset(nullptr);
        mScreenTexture.reset(nullptr);
        
        // Cutting up and down texture height and width
        int adjustedWidth = AdjustWidth(width);
        int adjustedHeight = AdjustHeight(height);

        // NOTE
        // This is the only place we limit width and height
        if(ReallocationRequired(adjustedWidth, adjustedHeight)) {
            CreateScreenTexture(adjustedWidth, adjustedHeight);
            CreateScreenSurface(adjustedWidth, adjustedHeight);
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
        try {
            auto searchResult = mGMCache.find(filename);
            if(searchResult != mGMCache.end()) {
                return *searchResult->second;
            }
        
            CollectionDataPtr &&ptr = LoadCollectionData(filename);
            if(!ptr) {
                throw Castle::Exception("Unable to load collection", BOOST_CURRENT_FUNCTION, __FILE__, __LINE__);
            }
        
            const CollectionData &data = *ptr;
            mGMCache.insert(
                std::make_pair(filename, std::move(ptr)));

            return data;
        } catch(const std::exception &error) {
            std::cerr << "Query collection failed: " << error.what() << std::endl;
            throw;
        }
    }

    bool Renderer::CacheCollection(const fs::path &filename)
    {
        try {
            QueryCollection(filename);
            return true;
        } catch(const std::exception &error) {
            std::cerr << "Cache collection failed: " << error.what() << std::endl;
            throw;
        }
    }
    
    void Renderer::EnableClearScreen(bool on)
    {
        mScreenClear = on;
    }
} // namespace Render
