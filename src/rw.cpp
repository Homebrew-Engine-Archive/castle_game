#include "rw.h"
#include <memory>
#include <iostream>


RWPtr RWFromFileBuffer(const FileBuffer &buffer)
{
    RWPtr src(SDL_RWFromConstMem(buffer.Data(), buffer.Size()));
    return std::move(src);
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
    throw (std::runtime_error)
{
    ReadFile(filename, mode);
}

FileBuffer::FileBuffer(const std::string &filename, const char *mode)
    throw (std::runtime_error)
    : FileBuffer {filename.c_str(), mode}
{ }

FileBuffer::FileBuffer(const FilePath &path, const char *mode)
    throw (std::runtime_error)
    : FileBuffer {path.string(), mode}
{ }

FileBuffer::FileBuffer(SDL_RWops *src)
    throw (std::runtime_error)
{
    ReadRW(src);
}

void FileBuffer::ReadFile(const char *filename, const char *mode)
{
    RWPtr src(SDL_RWFromFile(filename, mode));
    ReadRW(src.get());
}

void FileBuffer::ReadRW(SDL_RWops *src)
{
    if(src == NULL)
        throw std::runtime_error("file not readable");
    
    Sint64 bytes = ReadableBytes(src);
    buffer.resize(bytes);
    if(SDL_RWread(src, Data(), sizeof(Uint8), bytes) != bytes) {
        throw std::runtime_error("eof unexpected");
    }
}

const Uint8 *FileBuffer::Data() const
{
    return &buffer[0];
}

Uint8 *FileBuffer::Data()
{
    return &buffer[0];
}

Sint64 FileBuffer::Size() const
{
    return buffer.size();
}
