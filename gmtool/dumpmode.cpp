#include "dumpmode.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/positional_options.hpp>

#include <gm1/gm1entryreader.h>
#include <gm1/gm1reader.h>
#include <gm1/gm1.h>

namespace po = boost::program_options;

namespace gmtool
{
    void DumpMode::GetOptions(boost::program_options::options_description &opts)
    {
        po::options_description mode("Dump mode");
        mode.add_options()
            ("file", po::value(&mInputFile)->required(), "Set .gm1 filename")
            ("index", po::value(&mEntryIndex)->required(), "Set entry index")
            ("tile-only", po::bool_switch(&mTileOnly), "Dump first 512 bytes of entry")
            ("box-only", po::bool_switch(&mBoxOnly), "Dump entry skipping 512 bytes at the beginning")
            ;
        opts.add(mode);
    }
    
    void DumpMode::GetPositionalOptions(boost::program_options::positional_options_description &unnamed)
    {
        unnamed.add("file", 1);
    }
    
    int DumpMode::Exec(const ModeConfig &cfg)
    {
        cfg.verbose << "Reading file " << mInputFile << std::endl;
        gm1::GM1Reader reader(mInputFile);
        cfg.verbose << "Collection has " << reader.NumEntries() << " entries" << std::endl;
        cfg.verbose << "ArchiveType is " << gm1::GetArchiveTypeName(reader.ArchiveType()) << std::endl;

        if(mEntryIndex >= reader.NumEntries()) {
            throw std::logic_error("Entry index is out of range");
        }

        if(mTileOnly && mBoxOnly) {
            throw std::logic_error("--tile-only and --box-only cannot be specified together");
        }

        if(reader.ArchiveType() != gm1::ArchiveType::TileObject) {
            if(mTileOnly) {
                cfg.verbose << "--tile-only ignored since incompatible encoding" << std::endl;
            }
            if(mBoxOnly) {
                cfg.verbose << "--box-only ignored since incompatible encoding" << std::endl;
            }
        }

        int64_t size = reader.EntrySize(mEntryIndex);
        int64_t offset = 0;
        
        /** Checking and choping entry size **/
        if(reader.ArchiveType() == gm1::ArchiveType::TileObject) {
            /** impossible or should be checked earlier? **/
            if(size < gm1::TileBytes) {
                std::ostringstream oss;
                oss << "Entry size is less than " << gm1::TileBytes;
                throw std::logic_error(oss.str());
            }
            
            if(mTileOnly) {
                size = gm1::TileBytes;
                offset = 0;
            } else if(mBoxOnly) {
                if(size == gm1::TileBytes) {
                    throw std::logic_error("No data to dump");
                }
                size = reader.EntrySize(mEntryIndex) - gm1::TileBytes;
                offset = gm1::TileBytes;
            }
        }
        
        if((mTileOnly) && (reader.ArchiveType() == gm1::ArchiveType::TileObject)) {
            cfg.verbose << "Dump tile data of size "
                        << size << " bytes"
                        << std::endl;
        } else if((mBoxOnly) && (reader.ArchiveType() == gm1::ArchiveType::TileObject)) {
            cfg.verbose << "Dump box of size "
                        << size << " bytes"
                        << std::endl;
        } else {
            cfg.verbose << "Dump whole entry data of size "
                        << size << " bytes"
                        << std::endl;
        }

        cfg.stdout.write(reader.EntryData(mEntryIndex) + offset, size);
        
        return EXIT_SUCCESS;
    }
}
