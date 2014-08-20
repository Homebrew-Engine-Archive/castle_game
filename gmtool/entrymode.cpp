#include "entrymode.h"

#include <iostream>
#include <stdexcept>

#include <gm1/gm1.h>
#include <gm1/gm1reader.h>

namespace gmtool
{
    int EntryMode::Exec(const ModeConfig &cfg)
    {
        cfg.verbose << "Reading file " << mInputFile << std::endl;
        gm1::GM1Reader reader(mInputFile);
        cfg.verbose << "Collection has " << reader.NumEntries() << " entries" << std::endl;
        
        if(mEntryIndex >= reader.NumEntries()) {
            throw std::runtime_error("Entry index is out of range");
        }

        const gm1::EntryHeader &header = reader.EntryHeader(mEntryIndex);
        if(!mBinary) {
            cfg.verbose << "Printing entry as text..." << std::endl;
            gm1::PrintEntryHeader(cfg.stdout, header);
        } else {
            cfg.verbose << "Printing entry as binary..." << std::endl;
            cfg.stdout.write(reinterpret_cast<char const*>(&header), gm1::CollectionEntryHeaderBytes);
        }
        
        return EXIT_SUCCESS;
    }
}
