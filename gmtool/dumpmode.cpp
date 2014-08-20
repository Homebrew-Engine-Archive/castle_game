#include "dumpmode.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include <gm1/gm1entryreader.h>
#include <gm1/gm1reader.h>
#include <gm1/gm1.h>

namespace gmtool
{
    int DumpMode::Exec(const ModeConfig &cfg)
    {
        cfg.verbose << "Reading file " << mInputFile << std::endl;
        gm1::GM1Reader reader(mInputFile);
        cfg.verbose << "Collection has " << reader.NumEntries() << " entries" << std::endl;
        cfg.verbose << "Using ReaderType: " << reader.GetEntryReader().GetName() << std::endl;

        if(mEntryIndex >= reader.NumEntries()) {
            throw std::runtime_error("Entry index is out of range");
        }

        int64_t offset = 0;                              // an offset from the beginning of entry
        int64_t size = reader.EntrySize(mEntryIndex);    // bytes count to be dumped

        // apply TilePart setting if possible
        if(reader.GetDataClass() == gm1::DataClass::TileBox) {
            cfg.verbose << "Set tile part setting to " << mTilePart << std::endl;
            
            if(size < gm1::TileBytes) {
                std::ostringstream oss;
                oss << "Entry size is less than " << gm1::TileBytes;
                throw std::runtime_error(oss.str());
            }

            switch(mTilePart) {
            case TilePart::Tile:
                size = gm1::TileBytes;
                offset = 0;
                cfg.verbose << "Dump tile data of size " << size << " bytes" << std::endl;
                break;
                
            case TilePart::Box:
                if(size <= gm1::TileBytes) {
                    throw std::runtime_error("No data to dump");
                }
                size = reader.EntrySize(mEntryIndex) - gm1::TileBytes;
                offset = gm1::TileBytes;
                cfg.verbose << "Dump box of size " << size << " bytes" << std::endl;
                break;
                
            case TilePart::Both:
                cfg.verbose << "Dump whole entry data of size " << size << " bytes" << std::endl;
                break;
                
            default:
                throw std::runtime_error("bad tile part");
            }
        } else {
            cfg.verbose << "Tile part setting is ignored since incompatible data class" << std::endl;
        }

        cfg.stdout.write(reader.EntryData(mEntryIndex) + offset, size);
        
        return EXIT_SUCCESS;
    }
}
