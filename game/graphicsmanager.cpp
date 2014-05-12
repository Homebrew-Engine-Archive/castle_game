#include "graphicsmanager.h"

#include <game/gm1palette.h>
#include <game/gm1entryreader.h>
#include <game/gm1reader.h>
#include <game/exception.h>

#include <boost/filesystem.hpp>

namespace Graphics
{
    GraphicsManager::GraphicsManager()
        : mGfxSurfaces()
        , mGm1Palettes()
        , mGm1Surfaces()
        , mGm1Centers()
    {
    }

    void GraphicsManager::CacheGM1(const std::string &gmname)
    {
        try {
            const fs::path gmpath = fs::GM1FilePath(gmname);
            GM1::GM1Reader gm1(gmpath, GM1::GM1Reader::Cached);

            std::vector<PalettePtr> &palettes = mGm1Palettes[gmname];
            std::vector<Surface> &surfaces = mGm1Surfaces[gmname];
            std::vector<Point> &centers = mGm1Centers[gmname];
            
            palettes.resize(gm1.NumPalettes());
            surfaces.resize(gm1.NumEntries());
            centers.resize(gm1.NumEntries());
            
            for(int i = 0; i < gm1.NumEntries(); ++i) {
                surfaces[i] = gm1.ReadEntry(i);
            }

            for(int i = 0; i < gm1.NumPalettes(); ++i) {
                palettes[i] = std::move(
                    GM1::CreateSDLPalette(
                        gm1.Palette(i)));
            }

            for(int i = 0; i < gm1.NumEntries(); ++i) {
                centers[i] = gm1.ImageCenter(i);
            }
            
        } catch(Castle::Error &error) {
            throw error
                ("gmname", gmname)
                ("file", __FILE__)
                ("line", std::to_string(__LINE__));
        } catch(const std::exception &error) {
            throw Castle::Error()
                ("what", error.what())
                ("gmname", gmname)
                ("file", __FILE__)
                ("line", std::to_string(__LINE__));
        }
    }
    
    Surface GraphicsManager::Gfx(const std::string &gfxname, bool cache)
    {
        try {
            if(cache && mGfxSurfaces.find(gfxname) != mGfxSurfaces.end()) {
                return mGfxSurfaces[gfxname];
            }
            
            const fs::path gfxpath = fs::TGXFilePath(gfxname);
            boost::filesystem::ifstream fin(gfxpath, std::ios_base::binary);
            Surface gfxSurface = TGX::ReadTGX(fin);

            if(cache) {
                mGfxSurfaces[gfxname] = gfxSurface;
            }

            return gfxSurface;

        } catch(Castle::Error &error) {
            throw error
                ("gfxname", gfxname)
                ("cache", std::to_string(cache));
        } catch(const std::exception &error) {
            throw Castle::Error()
                ("what", error.what())
                ("gfxname", gfxname)
                ("cache", std::to_string(cache));
        }        
    }

    Surface GraphicsManager::GetImage(const std::string &gmname, int entry)
    {
        return mGm1Surfaces[gmname].at(entry);
    }

    SDL_Palette* GraphicsManager::GetPalette(const std::string &gmpath, int palette)
    {
        return mGm1Palettes[gmpath].at(palette).get();
    }

    Point GraphicsManager::GetImageCenter(const std::string &gmname, int entry)
    {
        return mGm1Centers[gmname].at(entry);
    }
}
