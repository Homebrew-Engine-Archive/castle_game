#include "headermode.h"

#include <game/gm1reader.h>
#include <game/gm1writer.h>

namespace po = boost::program_options;

namespace GMTool
{
    void HeaderMode::GetOptions(po::options_description &opts)
    {
        po::options_description mode("Header mode");
        mode.add_options()
            ("file", po::value(&mInputFile)->required(), "Set .gm1 filename")
            ("binary", po::bool_switch(&mBinary), "Print header in binary format")
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
