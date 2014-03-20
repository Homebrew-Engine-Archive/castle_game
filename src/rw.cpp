#include "rw.h"
#include <memory>
#include <iostream>

TempSeek::TempSeek(SDL_RWops *src, int64_t whither, int seek)
    : mSrc(src)
    , mOrigin(SDL_RWtell(src))
{
    SDL_RWseek(mSrc, whither, seek);
}

TempSeek::~TempSeek()
{
    SDL_RWseek(mSrc, mOrigin, RW_SEEK_SET);
}

RWPtr RWFromFileBuffer(const FileBuffer &buffer)
{
    RWPtr src(SDL_RWFromConstMem(buffer.Data(), buffer.Size()));
    return std::move(src);
}

int64_t ReadableBytes(SDL_RWops *src)
{
    int64_t size = SDL_RWsize(src);
    int64_t pos = SDL_RWtell(src);
    return ((size < 0) || (pos < 0) || (size < pos))
        ? 0
        : size - pos;
}

void ReadInt32ArrayLE(SDL_RWops *src, uint32_t *buffer, size_t num)
{
    SDL_RWread(src, buffer, num, sizeof(uint32_t));

    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        for(uint32_t *ptr = buffer; ptr != buffer + num; ++ptr)
            *ptr = SDL_Swap32(*ptr);
    }
}

void ReadInt16ArrayLE(SDL_RWops *src, uint16_t *buffer, size_t num)
{
    SDL_RWread(src, buffer, num, sizeof(uint16_t));

    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        for(uint16_t *ptr = buffer; ptr != buffer + num; ++ptr)
            *ptr = SDL_Swap16(*ptr);
    }
}

void ReadInt8ArrayLE(SDL_RWops *src, uint8_t *buffer, size_t num)
{
    SDL_RWread(src, buffer, num, sizeof(uint8_t));
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
    
    int64_t bytes = ReadableBytes(src);
    buffer.resize(bytes);
    if(SDL_RWread(src, Data(), sizeof(uint8_t), bytes) != bytes) {
        throw std::runtime_error("eof unexpected");
    }
}

const uint8_t *FileBuffer::Data() const
{
    return &buffer[0];
}

uint8_t *FileBuffer::Data()
{
    return &buffer[0];
}

int64_t FileBuffer::Size() const
{
    return buffer.size();
}
