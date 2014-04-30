#include "filesystem.h"

#include <memory>
#include <SDL.h>

namespace fs
{

    path FXPath(const path &suffix)
    {
        return RootPath() / "fx" / suffix;
    }

    path GFXPath(const path &suffix)
    {
        return RootPath() / "gfx" / suffix;
    }

    path GMPath(const path &suffix)
    {
        return RootPath() / "gm" / suffix;
    }

    path FontsPath(const path &suffix)
    {
        return RootPath() / "fonts" / suffix;
    }

    path GM1FilePath(const std::string &filename, const std::string &extension)
    {
        path p = GMPath(filename);
        p += extension;
        return p;
    }

    path TGXFilePath(const std::string &filename, const std::string &extension)
    {
        path p = GFXPath(filename);
        p += extension;
        return p;
    }

    path FontFilePath(const std::string &filename, const std::string &extension)
    {
        path p = FontsPath(filename);
        p += extension;
        return p;
    }

    path RootPath()
    {
        static path staticBasePath(SDL_GetBasePath());
        return staticBasePath;
    }

    path PreferencesPath()
    {
        static path staticPrefPath(SDL_GetPrefPath("castlegame", "castlegame"));
        return staticPrefPath;
    }
}
