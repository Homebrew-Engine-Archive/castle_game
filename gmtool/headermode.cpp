#include "headermode.h"

#include <iostream>

#include <gm1/gm1writer.h>
#include <gm1/gm1reader.h>
#include <gm1/gm1entryreader.h>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/positional_options.hpp>

#include "modes.h"

namespace po = boost::program_options;

namespace gmtool
{
    void HeaderMode::GetOptions(po::options_description &opts)
    {
        po::options_description mode("Header mode");
        mode.add_options()
            (kFile,     po::value(&mInputFile)->required(),     "Set .gm1 filename")
            (kBinary,   po::bool_switch(&mBinary),              "Print header in binary format")
            (kCount,    po::bool_switch(&mCountRequested),      "Print entries count")
            (kType,     po::bool_switch(&mReaderTypeRequested), "Print prefered reader's type")
            ;
        opts.add(mode);
    }

    void HeaderMode::GetPositionalOptions(po::positional_options_description &unnamed)
    {
        unnamed.add(kFile, 1);
    }

    int HeaderMode::Exec(const ModeConfig &cfg)
    {
        cfg.verbose << "Reading file " << mInputFile << std::endl;
        gm1::GM1Reader reader(mInputFile);

        cfg.verbose << "Collection has " << reader.NumEntries() << " entries" << std::endl;

        const gm1::Header &header = reader.Header();

        if(mReaderTypeRequested) {
            cfg.stdout << reader.GetEntryReader().GetName() << std::endl;
            return EXIT_SUCCESS;
        }

        if(mCountRequested) {
            cfg.stdout << reader.NumEntries() << std::endl;
            return EXIT_SUCCESS;
        }

        if(!mBinary) {
            gm1::PrintHeader(cfg.stdout, header);
        } else {
            gm1::WriteHeader(cfg.stdout, reader.Header());
        }
        return EXIT_SUCCESS;
    }
}
