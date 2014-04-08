#ifndef GM1ENTRYWRITER_H_
#define GM1ENTRYWRITER_H_

#include <iosfwd>

namespace GM1
{

    class GM1EntryWriter
    {
    public:
        virtual std::ostream& WriteEntry(std::ostream &out);
    };
    
}

#endif
