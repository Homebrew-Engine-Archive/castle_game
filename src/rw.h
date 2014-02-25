#ifndef RW_H_
#define RW_H_

#include <string>
#include <vector>
#include <exception>
#include <memory>
#include <SDL2/SDL.h>

struct RWCloseDeleter
{
    void operator()(SDL_RWops *src) const;
};

struct file_not_readable { };

class FileBuffer
{
    std::vector<Uint8> buffer;

    void ReadFile(const char *filename, const char *mode);
    
public:
    FileBuffer(const char *filename, const char *mode) throw (file_not_readable);
    FileBuffer(const std::string &filename, const char *mode) throw (file_not_readable);
    ~FileBuffer();

    const Uint8 *to_uint8() const;
    Uint8 *to_uint8();
    Sint64 Size() const;
};


Sint64 ReadableBytes(SDL_RWops *src);

void ReadInt16Array(SDL_RWops *src, Uint16 *buffer, size_t num);
void ReadInt32Array(SDL_RWops *src, Uint32 *buffer, size_t num);
void ReadInt8Array(SDL_RWops *src, Uint8 *buffer, size_t num);

#endif
