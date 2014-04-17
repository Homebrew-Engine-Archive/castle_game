#include "palettemode.h"

#include <iostream>

#include <game/gm1writer.h>
#include <game/gm1reader.h>
#include <game/gm1palette.h>

namespace po = boost::program_options;

namespace GMTool
{
    void PaletteMode::GetOptions(po::options_description &opts)
    {
        po::options_description mode("Palette mode");
        mode.add_options()
            ("file", po::value(&mInputFile)->required(), "Set .gm1 filename")
            ("palette", po::value(&mPaletteIndex)->required(), "Set palette index")
            ("binary", po::bool_switch(&mBinary), "Dump palette in binary")
            ;
        opts.add(mode);
    }

    void PaletteMode::GetPositionalOptions(po::positional_options_description &unnamed)
    {
        unnamed.add("file", 1);
    }
    
    int PaletteMode::Exec(const ModeConfig &cfg)
    {
        cfg.verbose << "Reading file " << mInputFile << std::endl;
        GM1::GM1Reader reader(mInputFile);
        cfg.verbose << "Collection has " << reader.NumEntries() << " entries" << std::endl;
        cfg.verbose << "Collection has " << reader.NumPalettes() << " palettes" << std::endl;
       
        if(mPaletteIndex < 0 || mPaletteIndex >= reader.NumPalettes()) {
            throw std::logic_error("Palette index is out of range");
        }

        const GM1::Palette &palette = reader.Palette(mPaletteIndex);
        if(!mBinary) {
            cfg.verbose << "Printing palette as text" << std::endl;
            GM1::PrintPalette(cfg.stdout, palette);
        } else {
            cfg.verbose << "Printing palette as binary" << std::endl;
            GM1::WritePalette(cfg.stdout, palette);
        }
        
        return EXIT_SUCCESS;
    }
}
