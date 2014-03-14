#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <string>
#include <iostream>
#include <boost/filesystem.hpp>
#include "SDL.h"
#include "rw.h"

typedef boost::filesystem::path FilePath;

class FileSystem
{
public:
    FileSystem(const std::string &root, const std::string &pref);

    FilePath FXPath(const std::string &filename = std::string()) const;
    FilePath GFXPath(const std::string &filename = std::string()) const;
    FilePath GM1Path(const std::string &filename = std::string()) const;
    FilePath RootPath() const;
    FilePath PrefPath() const;
    
};

#endif
