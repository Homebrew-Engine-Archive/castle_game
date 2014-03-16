#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <memory>
#include <string>
#include <boost/filesystem.hpp>
#include "SDL.h"

using FilePath = boost::filesystem::path;

FilePath GetFXPath(const FilePath &path = FilePath());
FilePath GetGFXPath(const FilePath &path = FilePath());
FilePath GetGMPath(const FilePath &path = FilePath());

FilePath GetGM1FilePath(const FilePath &path, const FilePath &ext = ".gm1");
FilePath GetTGXFilePath(const FilePath &path, const FilePath &ext = ".tgx");

FilePath GetRootPath();
FilePath GetPrefPath();

#endif
