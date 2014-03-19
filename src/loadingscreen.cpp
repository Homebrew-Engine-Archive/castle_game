#include "loadingscreen.h"
#include "geometry.h"
#include "gm1.h"
#include "filesystem.h"
#include "SDL.h"
#include "collection.h"
#include "surface.h"
#include <boost/filesystem/fstream.hpp>
#include <memory>
#include <sstream>
#include <iostream>

using namespace std;

namespace
{

    vector<string> GetStringList(const FilePath &path)
    {
        vector<string> list;
    
        boost::filesystem::ifstream fin(path);
        string buffer;
        while(!fin.eof()) {
            getline(fin, buffer);
            if(!buffer.empty()) {
                list.push_back(buffer);
            }
        }

        return list;
    }

    template<class T>
    bool ReadVector(boost::filesystem::ifstream &in, vector<T> &xs)
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

    vector<FontCollectionInfo> GetFontCollectionInfoList(const FilePath &path)
    {
        boost::filesystem::ifstream fin(path);
        vector<FontCollectionInfo> fontsInfo;

        while(!fin.eof()) {
            FontCollectionInfo info;
            if(ParseFontCollectionInfo(fin, info)) {
                fontsInfo.push_back(info);
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
    : m_renderer(root->GetRenderer())
    , m_root(root)
    , m_quit(false)
{
    FilePath preloadsPath = GetGMPath("preloads.txt");
    for(const string &str : GetStringList(preloadsPath)) {
        ScheduleCacheGM1(str);
    }

    FilePath fontsPath = GetGMPath("fonts.txt");
    for(const FontCollectionInfo &info : GetFontCollectionInfoList(fontsPath)) {
        ScheduleCacheFont(info);
    }

    FilePath filepath = GetTGXFilePath("frontend_loading");
    m_background = m_renderer->QuerySurface(filepath);
}

void LoadingScreen::ScheduleCacheGM1(const FilePath &filename)
{
    FilePath path = GetGM1FilePath(filename);
    auto task = [path, this]() {
        if(!m_renderer->CacheCollection(path)) {
            ostringstream oss;
            oss << "Unable to load file: " << path;
            throw runtime_error(oss.str());
        }
    };
    m_tasks.push_back(task);
}

void LoadingScreen::ScheduleCacheFont(const FontCollectionInfo &info)
{
    auto task = [info, this]() {
        if(!m_renderer->CacheFontCollection(info)) {
            ostringstream oss;
            oss << "Unable to load file: " << info.filename;
            throw runtime_error(oss.str());
        }
    };
    m_tasks.push_back(task);
}

int LoadingScreen::Exec()
{
    const Uint32 frameRate = 5;
    const Uint32 frameInterval = 1000 / frameRate;
    Uint32 lastFrame = 0;
    const Uint32 total = m_tasks.size();
    Uint32 completed = 0;
        
    for(const auto &task : m_tasks) {
        if(lastFrame + frameInterval < SDL_GetTicks()) {
            lastFrame = SDL_GetTicks();
            double done = static_cast<double>(completed) / total;
            Draw(done);
        }

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(!m_root->HandleEvent(event))
                return -1;
        }

        task();
        completed += 1;
        
    }
    
    return 0;
}

void LoadingScreen::Draw(double done)
{
    Surface frame = m_renderer->BeginFrame();

    SDL_Rect frameRect = SurfaceBounds(frame);
    SDL_Rect bgRect = SurfaceBounds(m_background);

    SDL_Rect bgAligned = PutIn(bgRect, frameRect, 0, 0);
    SDL_BlitSurface(m_background, NULL, frame, &bgAligned);
    
    SDL_Rect barOuter = MakeRect(300, 25);
    SDL_Rect barOuterAligned = PutIn(barOuter, bgAligned, 0, 0.8f);
    FillFrame(frame, &barOuterAligned, 0x7f000000);
    DrawFrame(frame, &barOuterAligned, 0xff000000);

    SDL_Rect barOuterPadded = PadIn(barOuterAligned, 5);
    SDL_Rect barInner = MakeRect(barOuterPadded.w * done, barOuterPadded.h);
    SDL_Rect barInnerAligned = PutIn(barInner, barOuterPadded, -1.0f, 0);
    FillFrame(frame, &barInnerAligned, 0xff000000);
    
    m_renderer->EndFrame();
}
