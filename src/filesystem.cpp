#include "filesystem.h"

static FilePath SubPath(const FilePath &root, const FilePath &sub, const FilePath &filename)
{
    FilePath path = root;
    path /= sub;
    path += filename;
    return path;
}

FilePath GetFXPath(const FilePath &filename)
{
    return SubPath(GetRootPath(), "fx/", filename);
}

FilePath GetGFXPath(const FilePath &filename)
{
    return SubPath(GetRootPath(), "gfx/", filename);
}

FilePath GetGMPath(const FilePath &filename)
{
    return SubPath(GetRootPath(), "gm/", filename);
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
    static std::unique_ptr<char[]> ptr =
        std::unique_ptr<char[]>(SDL_GetBasePath());

    return FilePath(ptr.get());
}

FilePath GetPrefPath()
{
    static std::unique_ptr<char[]> ptr =
        std::unique_ptr<char[]>(SDL_GetPrefPath("stockade", "stockade"));
    
    return FilePath(ptr.get());
}
