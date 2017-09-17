#include "vfs.h"

#include <boost/algorithm/string.hpp>

#include <memory>
#include <SDL.h>

namespace castle
{
    namespace vfs
    {
        path GetFXPath(const path &suffix)
        {
            return GetRootPath() / "fx" / suffix;
        }

        path GetGFXPath(const path &suffix)
        {
            return GetRootPath() / "gfx" / suffix;
        }

        path GetGMPath(const path &suffix)
        {
            return GetRootPath() / "gm" / suffix;
        }

        path GetTTFPath(const path &suffix)
        {
            return GetRootPath() / "fonts" / suffix;
        }

        path GetGM1Filename(const std::string &filename, const std::string &extension)
        {
            path p = GetGMPath(filename);
            p += extension;
            return p;
        }

        path GetTGXFilename(const std::string &filename, const std::string &extension)
        {
            path p = GetGFXPath(filename);
            p += extension;
            return p;
        }

        path GetTTFFilename(const std::string &filename, const std::string &extension)
        {
            path p = GetTTFPath(filename);
            p += extension;
            return p;
        }

        path GetRootPath()
        {
            static path basePath(SDL_GetBasePath());
            return basePath;
        }

        path GetPreferencesPath()
        {
            static path prefPath(SDL_GetPrefPath("castlegame", "castlegame"));
            return prefPath;
        }

        const std::vector<path> BuildTTFPathList(const std::string &fontName)
        {
            return std::vector<path> {
                GetTTFFilename(fontName),
                GetTTFFilename(boost::to_upper_copy(fontName)),
                GetTTFFilename(boost::to_lower_copy(fontName))
            };
        }
    }
}
