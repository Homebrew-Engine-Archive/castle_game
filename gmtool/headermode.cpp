#include "headermode.h"

#include <iostream>

#include <game/gm1writer.h>
#include <game/gm1reader.h>

namespace po = boost::program_options;

namespace GMTool
{
    void HeaderMode::GetOptions(po::options_description &opts)
    {
        po::options_description mode("Header mode");
        mode.add_options()
            ("file", po::value(&mInputFile)->required(), "Set .gm1 filename")
            ("binary", po::bool_switch(&mBinary), "Print header in binary format")
            ("count", po::bool_switch(&mCountRequested), "Print entries count")
            ("encoding", po::bool_switch(&mEncodingRequested), "Print encoding name")
            ;
        opts.add(mode);
    }

    void HeaderMode::GetPositionalOptions(po::positional_options_description &unnamed)
    {
        unnamed.add("file", 1);
    }
    
    int HeaderMode::Exec(const ModeConfig &cfg)
    {
        cfg.verbose << "Reading file " << mInputFile << std::endl;
        GM1::GM1Reader reader(mInputFile);
        cfg.verbose << "Collection has " << reader.NumEntries() << " entries" << std::endl;

        const GM1::Header &header = reader.Header();

        if(mEncodingRequested) {
            cfg.verbose << "Print collections's encoding" << std::endl;
            cfg.stdout << GM1::GetEncodingName(reader.Encoding()) << std::endl;
            return EXIT_SUCCESS;
        }
        
        if(mCountRequested) {
            cfg.verbose << "Print entries length only" << std::endl;
            cfg.stdout << reader.NumEntries() << std::endl;
            return EXIT_SUCCESS;
        }
        
        if(!mBinary) {
            cfg.verbose << "Printing header in text format" << std::endl;
            GM1::PrintHeader(cfg.stdout, header);
        } else {
            cfg.verbose << "Printing header in binary format" << std::endl;
            GM1::WriteHeader(cfg.stdout, reader.Header());
        }
        return EXIT_SUCCESS;
    }
}
