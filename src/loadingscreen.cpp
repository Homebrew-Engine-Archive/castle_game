#include "loadingscreen.h"
#include "geometry.h"
#include "gm1.h"
#include "filesystem.h"
#include "SDL.h"
#include "collection.h"
#include <memory>
#include <sstream>
#include <iostream>
#include <boost/filesystem/fstream.hpp>

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

int RunLoadingScreen(RootScreen *root)
{
    LoadingScreen ls(root);
    return ls.Exec();
}

LoadingScreen::LoadingScreen(RootScreen *root)
    : mRenderer(root->GetRenderer())
    , mRoot(root)
    , mQuit(false)
{
    FilePath preloadsPath = GetGMPath("preloads.txt");
    for(const std::string &str : GetStringList(preloadsPath)) {
        ScheduleCacheGM1(str);
    }

    FilePath fontsPath = GetGMPath("fonts.txt");
    for(const FontCollectionInfo &info : GetFontCollectionInfoList(fontsPath)) {
        ScheduleCacheFont(info);
    }

    FilePath filepath = GetTGXFilePath("frontend_loading");
    mBackground = mRenderer->QuerySurface(filepath);
}

void LoadingScreen::ScheduleCacheGM1(const FilePath &filename)
{
    FilePath path = GetGM1FilePath(filename);
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

int LoadingScreen::Exec()
{
    const uint32_t frameRate = 5;
    const uint32_t frameInterval = 1000 / frameRate;
    uint32_t lastFrame = 0;
    const uint32_t total = mTasks.size();
    uint32_t completed = 0;
        
    for(const auto &task : mTasks) {
        if(lastFrame + frameInterval < SDL_GetTicks()) {
            lastFrame = SDL_GetTicks();
            double done = static_cast<double>(completed) / total;
            Draw(done);
        }

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(!mRoot->HandleEvent(event))
                return -1;
        }

        task();
        completed += 1;
        
    }
    
    return 0;
}

void LoadingScreen::Draw(double done)
{
    Surface frame = mRenderer->BeginFrame();

    SDL_Rect frameRect = SurfaceBounds(frame);
    SDL_Rect bgRect = SurfaceBounds(mBackground);

    SDL_Rect bgAligned = PutIn(bgRect, frameRect, 0, 0);
    SDL_BlitSurface(mBackground, NULL, frame, &bgAligned);
    
    SDL_Rect barOuter = MakeRect(300, 25);
    SDL_Rect barOuterAligned = PutIn(barOuter, bgAligned, 0, 0.8f);
    FillFrame(frame, &barOuterAligned, 0x7f000000);
    DrawFrame(frame, &barOuterAligned, 0xff000000);

    SDL_Rect barOuterPadded = PadIn(barOuterAligned, 5);
    SDL_Rect barInner = MakeRect(barOuterPadded.w * done, barOuterPadded.h);
    SDL_Rect barInnerAligned = PutIn(barInner, barOuterPadded, -1.0f, 0);
    FillFrame(frame, &barInnerAligned, 0xff000000);
    
    mRenderer->EndFrame();
}
