#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <boost/filesystem.hpp>
#include <string>

namespace fs
{
    using path = boost::filesystem::wpath;

    path FXPath(const path &suffix = path());
    path GFXPath(const path &suffix = path());
    path GMPath(const path &suffix = path());
    path FontsPath(const path &suffix = path());

    path GM1FilePath(const std::string &fileName, const std::string &extension = ".gm1");
    path TGXFilePath(const std::string &fileName, const std::string &extension = ".tgx");
    path FontFilePath(const std::string &fileName, const std::string &extension = ".ttf");

    path RootPath();
    path PreferencesPath();
}

#endif
