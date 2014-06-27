#include "rw.h"

#include <iostream>

namespace
{
    template<class StreamModel>
    void SeekStream(StreamModel &stream, std::streamoff off, std::ios_base::seekdir way);
    
    template<class StreamModel>
    int64_t TellStream(StreamModel &stream);
    
    template<class StreamModel>
    void ReadStream(StreamModel &stream, char *data, std::streamsize size);
    
    template<class StreamModel>
    void WriteStream(StreamModel &stream, const char *data, std::streamsize size);
    
    template<> void SeekStream(std::basic_istream<char> &in, std::streamoff off, std::ios_base::seekdir way)
    {
        in.seekg(off, way);
    }
    
    template<> void SeekStream(std::basic_ostream<char> &out, std::streamoff off, std::ios_base::seekdir way)
    {
        out.seekp(off, way);
    }

    template<> int64_t TellStream(std::basic_istream<char> &in)
    {
        return static_cast<int64_t>(in.tellg());
    }
    
    template<> int64_t TellStream(std::basic_ostream<char> &out)
    {
        return static_cast<int64_t>(out.tellp());
    }

    template<> void ReadStream(std::basic_istream<char> &in, char *data, std::streamsize size)
    {
        in.read(data, size);
    }
    
    template<> void ReadStream(std::basic_ostream<char> &out, char*, std::streamsize)
    {
        perror("Read from write-only stream");
        out.setstate(std::ios_base::failbit);
    }
    
    template<> void WriteStream(std::basic_istream<char> &in, char const*, std::streamsize)
    {
        perror("Write to read-only stream");
        in.setstate(std::ios_base::failbit);
    }
    
    template<> void WriteStream(std::basic_ostream<char> &out, const char *data, std::streamsize size)
    {
        out.write(data, size);
    }

    int Close(SDL_RWops *context)
    {
        if(context != NULL) {
            SDL_FreeRW(context);
        }
        return 0;
    }
    
    std::ios_base::seekdir ConvertSeekDirection(int whence)
    {
        switch(whence) {
        case RW_SEEK_SET: return std::ios_base::beg;
        case RW_SEEK_END: return std::ios_base::end;
        case RW_SEEK_CUR: 
        default:
            return std::ios_base::cur;
        }
    }
    
    template<class StreamModel>
    int64_t Seek(SDL_RWops *context, int64_t offset, int whence)
    {
        StreamModel &stream = *reinterpret_cast<StreamModel*>(context->hidden.unknown.data1);

        SeekStream<StreamModel>(stream, offset, ConvertSeekDirection(whence));
        
        const int64_t pos = TellStream<StreamModel>(stream);
        // ok, if stream failbit is up pos is -1
        return pos;
    }

    template<class StreamModel>
    size_t Read(SDL_RWops *context, void *data, size_t size, size_t maxnum)
    {
        StreamModel &stream = *reinterpret_cast<StreamModel*>(context->hidden.unknown.data1);
        
        const int64_t before = TellStream<StreamModel>(stream);
        ReadStream<StreamModel>(stream, reinterpret_cast<char*>(data), size * maxnum);
        const int64_t after = TellStream<StreamModel>(stream);
        
        return (stream ? ((after - before) / size) : 0);
    }

    template<class StreamModel>
    size_t Write(SDL_RWops *context, const void *data, size_t size, size_t maxnum)
    {
        StreamModel &stream = *reinterpret_cast<StreamModel*>(context->hidden.unknown.data1);
        
        const int64_t before = TellStream<StreamModel>(stream);
        WriteStream<StreamModel>(stream, reinterpret_cast<char const*>(data), size * maxnum);
        const int64_t after = TellStream<StreamModel>(stream);
        
        return (stream ? ((after - before) / size) : 0);
    }
    
    template<class StreamModel>
    SDL_RWops* SDL_RWFromStream(StreamModel &stream)
    {
        SDL_RWops *rw = SDL_AllocRW();
        if(rw != NULL) {
            rw->seek = Seek<StreamModel>;
            rw->read = Read<StreamModel>;
            rw->write = Write<StreamModel>;
            rw->close = Close;
            rw->type = SDL_RWOPS_UNKNOWN;
            rw->hidden.unknown.data1 = &stream;
        }
        return rw;
    }
}

namespace core
{
    SDL_RWops* SDL_RWFromInputStream(std::basic_istream<char> &is)
    {
        return SDL_RWFromStream<std::basic_istream<char>>(is);
    }

    SDL_RWops* SDL_RWFromOutputStream(std::basic_ostream<char> &os)
    {
        return SDL_RWFromStream<std::basic_ostream<char>>(os);
    }
}
