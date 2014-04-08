#ifndef GM1WRITER_H_
#define GM1WRITER_H_

#include <game/filesystem.h>
#include <game/gm1.h>

class Surface;

namespace GM1
{
    class GM1Reader;
    enum class Encoding;
    class GM1EntryWriter;
}

namespace GM1
{

    class GM1Writer
    {
    public:
        GM1Writer();
        GM1Writer(GM1::GM1Reader const&);

        void SetEncoding(GM1::Encoding const&);
        void SetAnchorX(int anchor);
        void SetAnchorY(int anchor);
        
        void InsertEntry(size_t index, GM1::EntryWriter const&);
        void Remove(size_t index);
        
        virtual ~GM1Writer();
    };
    
}

#endif
