#ifndef RW_H_
#define RW_H_

#include <string>
#include <vector>
#include <stdexcept>
#include <SDL.h>
#include "filesystem.h"
#include "sdl_utils.h"

class FileBuffer
{
    std::vector<uint8_t> buffer;
    void ReadRW(SDL_RWops *src);
    void ReadFile(const char *filename, const char *mode);
    
public:
    FileBuffer(const char *filename, const char *mode) throw (std::runtime_error);
    FileBuffer(const std::string &filename, const char *mode) throw (std::runtime_error);
    FileBuffer(const FilePath &path, const char *mode) throw (std::runtime_error);
    FileBuffer(SDL_RWops *src) throw (std::runtime_error);

    const uint8_t *Data() const;
    uint8_t *Data();
    int64_t Size() const;
};

class TempSeek
{
    SDL_RWops *mSrc;
    int64_t mOrigin;
public:
    TempSeek(SDL_RWops *src, int64_t offset, int mode);
    ~TempSeek();
};

RWPtr RWFromFileBuffer(const FileBuffer &filebuffer);

int64_t ReadableBytes(SDL_RWops *src);

void ReadInt16ArrayLE(SDL_RWops *src, uint16_t *buffer, size_t num);
void ReadInt32ArrayLE(SDL_RWops *src, uint32_t *buffer, size_t num);
void ReadInt8ArrayLE(SDL_RWops *src, uint8_t *buffer, size_t num);

#endif
