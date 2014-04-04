#include "loadingscreen.h"

#include <memory>
#include <sstream>
#include <iostream>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/clamp.hpp>
#include <SDL.h>

#include <game/make_unique.h>
#include <game/collection.h>
#include <game/filesystem.h>
#include <game/geometry.h>
#include <game/renderer.h>
#include <game/engine.h>

namespace
{

    std::vector<std::string> GetStringList(const FilePath &path)
    {
        std::vector<std::string> list;
    
        boost::filesystem::ifstream fin(path);
        std::string buffer;
        while(!fin.eof()) {
            getline(fin, buffer);
            if(!buffer.empty()) {
                list.push_back(buffer);
            }
        }

        return list;
    }

    template<class T>
    bool ReadVector(boost::filesystem::ifstream &in, std::vector<T> &xs)
    {
        if(in.eof())
            return false;

        int n;
        in >> n;
        xs.resize(n);
        for(T &x : xs) {
            if(in.eof())
                return false;
            else
                in >> x;
        }

        return true;
    }

    bool ParseFontCollectionInfo(boost::filesystem::ifstream &in, FontCollectionInfo &info)
    {
        in >> info.name;
        info.filename = GetGM1FilePath(info.name);

        if(!ReadVector(in, info.alphabet))
            return false;
        if(!ReadVector(in, info.sizes))
            return false;

        return true;
    }

    std::vector<FontCollectionInfo> GetFontCollectionInfoList(const FilePath &path)
    {
        boost::filesystem::ifstream fin(path);
        std::vector<FontCollectionInfo> fontsInfo;

        while(!fin.eof()) {
            FontCollectionInfo info;
            if(ParseFontCollectionInfo(fin, info)) {
                fontsInfo.push_back(std::move(info));
            }
        }
    
        return fontsInfo;
    }

}

namespace UI
{

    LoadingScreen::LoadingScreen(Castle::Engine *engine)
        : mRenderer(engine->GetRenderer())
        , mEngine(engine)
        , mBackground(NULL)
        , mProgressDone(0)
        , mProgressMax(1)
        , mDirty(false)
    {
        const FilePath fontsPath = GetGMPath("fonts.txt");
        for(const FontCollectionInfo &info : GetFontCollectionInfoList(fontsPath)) {
            ScheduleCacheFont(info);
        }
        
        const FilePath preloadsPath = GetGMPath("preloads.txt");
        for(const std::string &str : GetStringList(preloadsPath)) {
            ScheduleCacheGM1(str);
        }

        const FilePath filepath = GetTGXFilePath("frontend_loading");
        mBackground = LoadSurface(filepath);
    }

    void LoadingScreen::ScheduleCacheGM1(const FilePath &filename)
    {
        const FilePath path = GetGM1FilePath(filename);
        auto task = [path, this]() {
            if(!mRenderer->CacheCollection(path)) {
                std::ostringstream oss;
                oss << "Unable to load file: " << path;
                throw std::runtime_error(oss.str());
            }
        };
        mTasks.push_back(task);
    }

    void LoadingScreen::ScheduleCacheFont(const FontCollectionInfo &info)
    {
        auto task = [info, this]() {
            if(!mRenderer->CacheFontCollection(info)) {
                std::ostringstream oss;
                oss << "Unable to load file: " << info.filename;
                throw std::runtime_error(oss.str());
            }
        };
        mTasks.push_back(task);
    }

    bool LoadingScreen::Exec()
    {
        const uint32_t frameRate = 5;
        const uint32_t frameInterval = 1000 / frameRate;
        uint32_t lastFrame = 0;
        uint32_t completed = 0;
        const uint32_t total = mTasks.size();
        SetProgressMax(total);
        
        for(const auto &task : mTasks) {
            if(lastFrame + frameInterval < SDL_GetTicks()) {
                lastFrame = SDL_GetTicks();
                SetProgressDone(completed);
                Surface frame = mRenderer->BeginFrame();
                Draw(frame);
                mRenderer->EndFrame();
            }

            mEngine->PollInput();
            if(mEngine->Closed())
                return false;

            task();
            completed += 1;
        }
    
        return true;
    }

    bool LoadingScreen::HandleEvent(SDL_Event const&)
    {
        return false;
    }

    bool LoadingScreen::IsDirty(int64_t elapsed) const
    {
        return (mDirty) && (elapsed > 250 /* ms */);
    }
    
    void LoadingScreen::SetProgressDone(int done)
    {
        if(mProgressDone != done) {
            mProgressDone = done;
            mDirty = true;
        }
    }

    void LoadingScreen::SetProgressMax(int max)
    {
        if(max <= 0) {
            throw std::runtime_error("Bad max");
        }
        if(mProgressMax != max) {
            mProgressMax = max;
            mDirty = true;
        }
    }

    void LoadingScreen::SetProgressLabel(std::string const& text)
    {
        std::cout << "Now loading: " << text << std::endl;
    }
    
    double LoadingScreen::GetCompleteRate() const
    {
        double done = static_cast<double>(mProgressDone) / mProgressMax;
        return boost::algorithm::clamp(done, 0.0f, 1.0f);
    }
    
    void LoadingScreen::Draw(Surface &frame)
    {
        mDirty = false;
        double rate = GetCompleteRate();
        
        SDL_Rect frameRect = SurfaceBounds(frame);
        SDL_Rect bgRect = SurfaceBounds(mBackground);

        SDL_Rect bgAligned = PutIn(bgRect, frameRect, 0, 0);
        SDL_BlitSurface(mBackground, NULL, frame, &bgAligned);
    
        SDL_Rect barOuter = MakeRect(300, 25);
        SDL_Rect barOuterAligned = PutIn(barOuter, bgAligned, 0, 0.8f);
        FillFrame(frame, &barOuterAligned, 0x7f000000);
        DrawFrame(frame, &barOuterAligned, 0xff000000);

        SDL_Rect barOuterPadded = PadIn(barOuterAligned, 5);
        SDL_Rect barInner = MakeRect(barOuterPadded.w * rate, barOuterPadded.h);
        SDL_Rect barInnerAligned = PutIn(barInner, barOuterPadded, -1.0f, 0);
        FillFrame(frame, &barInnerAligned, 0xff000000);
    }

    std::unique_ptr<UI::LoadingScreen> CreateLoadingScreen(Castle::Engine *engine)
    {
        return std::make_unique<UI::LoadingScreen>(engine);
    }
        
} // namespace UI
