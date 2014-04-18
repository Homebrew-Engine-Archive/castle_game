#include "rw.h"

#include <iostream>

namespace general
{
    template<class T> void Seek(T &stream, std::streamoff off, std::ios_base::seekdir way);
    template<class T> int64_t Tell(T &stream);
    template<class T> void Read(T &stream, char *data, std::streamsize size);
    template<class T> void Write(T &stream, const char *data, std::streamsize size);
    
    template<> void Seek(std::istream &in, std::streamoff off, std::ios_base::seekdir way)
    {
        in.seekg(off, way);
    }
    template<> void Seek(std::ostream &out, std::streamoff off, std::ios_base::seekdir way)
    {
        out.seekp(off, way);
    }

    template<> int64_t Tell(std::istream &in)
    {
        return static_cast<int64_t>(in.tellg());
    }
    template<> int64_t Tell(std::ostream &out)
    {
        return static_cast<int64_t>(out.tellp());
    }

    template<> void Read(std::istream &in, char *data, std::streamsize size)
    {
        in.read(data, size);
    }
    template<> void Read(std::ostream &out, char*, std::streamsize)
    {
        perror("Read from write-only stream");
        out.setstate(std::ios_base::failbit);
    }
    
    template<> void Write(std::istream &in, char const*, std::streamsize)
    {
        perror("Write to read-only stream");
        in.setstate(std::ios_base::failbit);
    }
    template<> void Write(std::ostream &out, const char *data, std::streamsize size)
    {
        out.write(data, size);
    }
}

namespace
{
    template<class T>
    int64_t Seek(SDL_RWops *context, int64_t offset, int whence)
    {
        T *stream = reinterpret_cast<T*>(context->hidden.unknown.data1);
        
        switch(whence) {
        case RW_SEEK_CUR:
            general::Seek<T>(*stream, offset, std::ios_base::cur);
            break;
            
        case RW_SEEK_SET:
            general::Seek<T>(*stream, offset, std::ios_base::beg);
            break;
            
        case RW_SEEK_END:
            general::Seek<T>(*stream, offset, std::ios_base::end);
            break;
            
        default:
            break;
        }

        if(stream->fail()) {
            return -1;
        }

        return general::Tell<T>(*stream);
    }

    template<class T>
    size_t Read(SDL_RWops *context, void *ptr, size_t size, size_t maxnum)
    {
        T *stream = reinterpret_cast<T*>(context->hidden.unknown.data1);

        int64_t origin = general::Tell<T>(*stream);
        general::Read<T>(*stream, reinterpret_cast<char*>(ptr), size * maxnum);
        int64_t latter = general::Tell<T>(*stream);

        return ((*stream) ? ((latter - origin) / size) : 0);
    }

    template<class T>
    size_t Write(SDL_RWops *context, const void *ptr, size_t size, size_t maxnum)
    {
        T *stream = reinterpret_cast<T*>(context->hidden.unknown.data1);

        int64_t origin = general::Tell<T>(*stream);
        general::Write<T>(*stream, reinterpret_cast<char const*>(ptr), size * maxnum);
        int64_t latter = general::Tell<T>(*stream);
        
        return ((*stream) ? ((latter - origin) / size) : 0);
    }
}

namespace
{
    int Close(SDL_RWops *context)
    {
        if(context != NULL) {
            SDL_FreeRW(context);
        }
        return 0;
    }
}

template<class T>
SDL_RWops* SDL_RWFromStream(T &stream)
{
    SDL_RWops *rw = SDL_AllocRW();
    if(rw != NULL) {
        rw->seek = Seek<T>;
        rw->read = Read<T>;
        rw->write = Write<T>;
        rw->close = Close;
        rw->type = SDL_RWOPS_UNKNOWN;
        rw->hidden.unknown.data1 = &stream;
    }
    return rw;
}

SDL_RWops* SDL_RWFromInputStream(std::istream &in)
{
    return SDL_RWFromStream<std::istream>(in);
}

SDL_RWops* SDL_RWFromOutputStream(std::ostream &out)
{
    return SDL_RWFromStream<std::ostream>(out);
}
