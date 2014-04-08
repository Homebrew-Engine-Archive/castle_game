#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <boost/filesystem.hpp>

using FilePath = boost::filesystem::wpath;

FilePath GetFXPath(const FilePath &path = FilePath());
FilePath GetGFXPath(const FilePath &path = FilePath());
FilePath GetGMPath(const FilePath &path = FilePath());

FilePath GetGM1FilePath(const FilePath &path, const FilePath &ext = ".gm1");
FilePath GetTGXFilePath(const FilePath &path, const FilePath &ext = ".tgx");

FilePath GetRootPath();
FilePath GetPrefPath();

#endif
