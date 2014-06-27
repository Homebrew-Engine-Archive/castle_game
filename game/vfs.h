#ifndef VFS_H_
#define VFS_H_

#include <vector>
#include <string>

#include <boost/filesystem/path.hpp>

namespace castle
{
    namespace vfs
    {
        using path = boost::filesystem::wpath;

        path GetFXPath(const path &suffix = path());
        path GetGFXPath(const path &suffix = path());
        path GetGMPath(const path &suffix = path());
        path GetTTFPath(const path &suffix = path());

        path GetGM1Filename(const std::string &fileName, const std::string &extension = ".gm1");
        path GetTGXFilename(const std::string &fileName, const std::string &extension = ".tgx");
        path GetTTFFilename(const std::string &fileName, const std::string &extension = ".ttf");

        const std::vector<path> BuildTTFPathList(const std::string &fontName);
        
        path GetRootPath();
        path GetPreferencesPath();
    }
}

#endif  // VFS_H_
