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
            ("i,index", po::value(&mEntryIndex)->required(), "Set entry index")
            ("tile-mode", po::value(&mTileRenderMode)->default_value(TileRenderMode::Default), "What part of tile will be dumped (default, tile or box)")
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
        cfg.verbose << "Using ReaderType: " << reader.GetEntryReader().GetName() << std::endl;

        if(mEntryIndex >= reader.NumEntries()) {
            throw std::runtime_error("Entry index is out of range");
        }

        int64_t offset = 0;                              // an offset from the beginning of entry
        int64_t size = reader.EntrySize(mEntryIndex);    // bytes count to be dumped

        // apply TileRenderMode if possible
        if(reader.GetDataClass() == gm1::DataClass::TileBox) {
            cfg.verbose << "Set tile render mode to " << mTileRenderMode << std::endl;
            
            if(size < gm1::TileBytes) {
                std::ostringstream oss;
                oss << "Entry size is less than " << gm1::TileBytes;
                throw std::runtime_error(oss.str());
            }

            switch(mTileRenderMode) {
            case TileRenderMode::Tile:
                size = gm1::TileBytes;
                offset = 0;
                cfg.verbose << "Dump tile data of size " << size << " bytes" << std::endl;
                break;
                
            case TileRenderMode::Box:
                if(size <= gm1::TileBytes) {
                    throw std::runtime_error("No data to dump");
                }
                size = reader.EntrySize(mEntryIndex) - gm1::TileBytes;
                offset = gm1::TileBytes;
                cfg.verbose << "Dump box of size " << size << " bytes" << std::endl;
                break;
                
            case TileRenderMode::Default:
                cfg.verbose << "Dump whole entry data of size " << size << " bytes" << std::endl;
                break;
                
            default:
                throw std::runtime_error("bad tile render mode");
            }
        } else {
            cfg.verbose << "Tile render mode ignored since incompatible data class" << std::endl;
        }

        cfg.stdout.write(reader.EntryData(mEntryIndex) + offset, size);
        
        return EXIT_SUCCESS;
    }
}
