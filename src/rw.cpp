#include "rw.h"

void RWCloseDeleter::operator()(SDL_RWops *src) const
{
    SDL_RWclose(src);
}

Sint64 ReadableBytes(SDL_RWops *src)
{
    Sint64 size = SDL_RWsize(src);
    Sint64 pos = SDL_RWtell(src);
    return ((size < 0) || (pos < 0) || (size < pos))
        ? 0
        : size - pos;
}

void ReadInt32ArrayLE(SDL_RWops *src, Uint32 *buffer, size_t num)
{
    SDL_RWread(src, buffer, num, sizeof(Uint32));

    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        for(Uint32 *ptr = buffer; ptr != buffer + num; ++ptr)
            *ptr = SDL_Swap32(*ptr);
    }
}

void ReadInt16ArrayLE(SDL_RWops *src, Uint16 *buffer, size_t num)
{
    SDL_RWread(src, buffer, num, sizeof(Uint16));

    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        for(Uint16 *ptr = buffer; ptr != buffer + num; ++ptr)
            *ptr = SDL_Swap16(*ptr);
    }
}

void ReadInt8ArrayLE(SDL_RWops *src, Uint8 *buffer, size_t num)
{
    SDL_RWread(src, buffer, num, sizeof(Uint8));

}

FileBuffer::FileBuffer(const char *filename, const char *mode)
    throw (file_not_readable)
{
    ReadFile(filename, mode);
}

FileBuffer::FileBuffer(const std::string &filename, const char *mode)
    throw (file_not_readable)
{
    ReadFile(filename.c_str(), mode);
}

void FileBuffer::ReadFile(const char *filename, const char *mode)
{
    try {
        SDL_RWops *src = SDL_RWFromFile(filename, mode);
        std::unique_ptr<SDL_RWops, RWCloseDeleter> autodeleter(src);
        
        if(src == NULL)
            throw file_not_readable();
        
        Sint64 size = SDL_RWsize(src);
        buffer.resize(size);
        if(SDL_RWread(src, to_uint8(), sizeof(Uint8), size) != size)
            throw file_not_readable();
        
    } catch(const std::exception &e) {
        SDL_Log("Exception in FileBuffer: %s", e.what());
        throw file_not_readable();
    } catch(...) {
        SDL_Log("Unknown exception in FileBuffer");
        throw file_not_readable();
    }
}

const Uint8 *FileBuffer::to_uint8() const
{
    return &buffer[0];
}

Uint8 *FileBuffer::to_uint8()
{
    return &buffer[0];
}

Sint64 FileBuffer::Size() const
{
    return buffer.size();
}

FileBuffer::~FileBuffer()
{
}
