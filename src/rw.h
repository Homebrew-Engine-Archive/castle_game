#ifndef RW_H_
#define RW_H_

#include <exception>
#include <memory>
#include <SDL2/SDL.h>

struct RWCloseDeleter
{
    void operator()(SDL_RWops *src) const;
};

struct IOException : public std::exception
{
    const char * what_arg;
    explicit IOException(const char *what_arg)
        throw()
        : what_arg(what_arg)
        {}
    virtual const char *what() const throw() {
        return what_arg;
    }
};

Uint32 ReadableBytes(SDL_RWops *src);

void ReadInt16Array(SDL_RWops *src, Uint16 *buffer, size_t num);
void ReadInt32Array(SDL_RWops *src, Uint32 *buffer, size_t num);
void ReadInt8Array(SDL_RWops *src, Uint8 *buffer, size_t num);

#endif
