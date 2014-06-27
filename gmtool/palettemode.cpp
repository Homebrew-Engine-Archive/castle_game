#include "palettemode.h"

#include <iostream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/positional_options.hpp>

#include <core/color.h>
#include <core/modulo.h>

#include <game/gm1writer.h>
#include <game/gm1reader.h>
#include <game/palette.h>

namespace po = boost::program_options;

namespace gmtool
{
    void PaletteMode::GetOptions(po::options_description &opts)
    {
        po::options_description mode("Palette mode");
        mode.add_options()
            ("file", po::value(&mInputFile)->required(), "Set .gm1 filename")
            ("palette", po::value(&mPaletteIndex), "Set palette index")
            ("binary", po::bool_switch(&mBinary), "Dump palette in binary")
            ("count", po::bool_switch(&mCountRequested), "Print palettes count")
            ;
        opts.add(mode);
    }

    void PaletteMode::GetPositionalOptions(po::positional_options_description &unnamed)
    {
        unnamed.add("file", 1);
    }

    std::ostream& PrintPalette(std::ostream &out, const castle::Palette &palette)
    {
        int column = 0;
        out << std::hex;
        for(castle::Palette::value_type entry : palette) {
            out << entry << ' ';
            ++column;
            if(core::Mod(column, 16) == 0) {
                out << std::endl;
            }
        }
        return out;
    }

    int PaletteMode::Exec(const ModeConfig &cfg)
    {
        cfg.verbose << "Reading file " << mInputFile << std::endl;
        gm1::GM1Reader reader(mInputFile);
        cfg.verbose << "Collection has " << reader.NumEntries() << " entries" << std::endl;
        cfg.verbose << "Collection has " << reader.NumPalettes() << " palettes" << std::endl;

        if(mCountRequested) {
            cfg.verbose << "Print palettes count only" << std::endl;
            cfg.stdout << reader.NumPalettes() << std::endl;
            return EXIT_SUCCESS;
        }
        
        if(mPaletteIndex >= reader.NumPalettes()) {
            throw std::logic_error("Palette index is out of range");
        }

        const castle::Palette &palette = reader.Palette(mPaletteIndex);
        if(!mBinary) {
            cfg.verbose << "Printing palette as text" << std::endl;
            PrintPalette(cfg.stdout, palette);
        } else {
            cfg.verbose << "Printing palette as binary" << std::endl;
            gm1::WritePalette(cfg.stdout, palette);
        }
        
        return EXIT_SUCCESS;
    }
}
