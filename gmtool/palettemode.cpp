#include "palettemode.h"

#include <iostream>

#include <core/color.h>
#include <core/modulo.h>
#include <core/palette.h>

#include <gm1/gm1writer.h>
#include <gm1/gm1reader.h>

namespace gmtool
{
    std::ostream& PrintPalette(std::ostream &out, const core::Palette &palette)
    {
        int column = 0;
        out << std::hex;
        for(core::Palette::value_type entry : palette) {
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
            throw std::runtime_error("Palette index is out of range");
        }

        const core::Palette &palette = reader.Palette(mPaletteIndex);
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
