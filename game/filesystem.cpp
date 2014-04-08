#include "filesystem.h"

#include <memory>
#include <SDL.h>

FilePath GetFXPath(const FilePath &filename)
{
    return GetRootPath() / "fx" / filename;
}

FilePath GetGFXPath(const FilePath &filename)
{
    return GetRootPath() / "gfx" / filename;
}

FilePath GetGMPath(const FilePath &filename)
{
    return GetRootPath() / "gm" / filename;
}

FilePath GetGM1FilePath(const FilePath &filename, const FilePath &ext)
{
    FilePath path = GetGMPath(filename);
    path += ext;
    return path;
}

FilePath GetTGXFilePath(const FilePath &filename, const FilePath &ext)
{
    FilePath path = GetGFXPath(filename);
    path += ext;
    return path;
}

FilePath GetRootPath()
{
    static FilePath staticBasePath(SDL_GetBasePath());
    return staticBasePath;
}

FilePath GetPrefPath()
{
    static FilePath staticPrefPath(SDL_GetPrefPath("castlegame", "castlegame"));
    return staticPrefPath;
}
