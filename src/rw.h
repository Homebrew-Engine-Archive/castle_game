#ifndef RW_H_
#define RW_H_

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include <SDL2/SDL.h>

struct RWCloseDeleter
{
    void operator()(SDL_RWops *src) const {
        if(src != NULL)
            SDL_RWclose(src);
    }
};

typedef std::unique_ptr<SDL_RWops, RWCloseDeleter> scoped_rwops;

class FileBuffer
{
    std::vector<Uint8> buffer;
    void ReadRW(SDL_RWops *src);
    void ReadFile(const char *filename, const char *mode);
    
public:
    FileBuffer(const char *filename, const char *mode) throw (std::runtime_error);
    FileBuffer(const std::string &filename, const char *mode) throw (std::runtime_error);
    FileBuffer(SDL_RWops *src) throw (std::runtime_error);
    ~FileBuffer();

    const Uint8 *Data() const;
    Uint8 *Data();
    Sint64 Size() const;
};

scoped_rwops RWFromFileBuffer(const FileBuffer &filebuffer);

Sint64 ReadableBytes(SDL_RWops *src);

void ReadInt16ArrayLE(SDL_RWops *src, Uint16 *buffer, size_t num);
void ReadInt32ArrayLE(SDL_RWops *src, Uint32 *buffer, size_t num);
void ReadInt8ArrayLE(SDL_RWops *src, Uint8 *buffer, size_t num);

#endif
