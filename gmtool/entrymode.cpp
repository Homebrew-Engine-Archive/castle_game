#include "entrymode.h"

#include <iostream>
#include <stdexcept>

#include <gm1/gm1.h>
#include <gm1/gm1reader.h>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/positional_options.hpp>

#include "modes.h"

namespace po = boost::program_options;

namespace gmtool
{
    void EntryMode::GetOptions(po::options_description &opts)
    {
        po::options_description mode("Entry mode");
        mode.add_options()
            (kFile,     po::value(&mInputFile)->required(),     "Set .gm1 filename")
            (kIndex,    po::value(&mEntryIndex)->required(),    "Set entry index")
            (kBinary,   po::bool_switch(&mBinary),              "Dump entry in binary")
            ;
        opts.add(mode);
    }

    void EntryMode::GetPositionalOptions(po::positional_options_description &unnamed)
    {
        unnamed.add(kFile, 1);
        unnamed.add(kIndex, 1);
    }

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
