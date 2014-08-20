#include "rendermode.h"

#include <string>
#include <sstream>
#include <cstring>

#include <boost/filesystem/fstream.hpp>

#include <gmtool/imagewriter.h>
#include <gmtool/table.h>

#include <gm1/gm1.h>
#include <gm1/gm1reader.h>
#include <gm1/gm1entryreader.h>

#include <core/image.h>
#include <core/palette.h>
#include <core/color.h>
#include <core/rect.h>
#include <core/rw.h>

namespace gmtool
{
    RenderMode::~RenderMode() throw() = default;
    RenderMode::RenderMode()
    {
        mFormats = RenderFormats();
    }

    void RenderMode::PrintUsage(std::ostream &out)
    {
        out << "Allowed render formats are:" << std::endl;

        Table table;
        table.AppendColumn("Format", Alignment::Left);
        for(const RenderFormat &format : mFormats) {
            table.AppendRow({format.name});
        }
        table.Print(out);
    }

    const core::Color RenderMode::DefaultTransparent() const
    {
        return core::Color(255, 0, 255, 255);
    }
    
    void RenderMode::SetupPalette(core::Image &image, const core::Palette &palette)
    {
        if(core::IsPalettized(image)) {
            core::Palette copied = palette;
            image.AttachPalette(copied);
        }
    }

    void RenderMode::SetupFormat(core::Image &image, uint32_t format)
    {
        if(core::IsPalettized(image)) {
            if(format != core::ImageFormat(image).format) {
                image = core::ConvertImage(image, format);
            }
        }
    }

    void RenderMode::SetupTransparentColor(core::Image &surface, const core::Color &color)
    {
        surface.SetColorKey(color);
    }
    
    int RenderMode::Exec(const ModeConfig &cfg)
    {
        int flags = gm1::GM1Reader::NoFlags;
        if(mTilePart == TilePart::Tile) {
            flags |= gm1::GM1Reader::SkipBox;
        } else if(mTilePart == TilePart::Box) {
            flags |= gm1::GM1Reader::SkipTile;
        }
        cfg.verbose << "Tile part setting set to " << mTilePart << std::endl;
        
        cfg.verbose << "Reading file " << mInputFile << std::endl;
        gm1::GM1Reader reader(mInputFile, flags);
        cfg.verbose << "Collection contains " << reader.NumEntries() << " entries" << std::endl;
        cfg.verbose << "Using ReaderType: " << reader.GetEntryReader().GetName() << std::endl;
                    if(mEntryIndex >= reader.NumEntries()) {
            throw std::runtime_error("Entry index is out of range");
        }

        if(mPaletteIndex >= reader.NumPalettes()) {
            throw std::runtime_error("Palette index is out of range");
        }

        if(DefaultTransparent() != mTransparentColor) {
            reader.SetTransparentColor(mTransparentColor);
        }
        cfg.verbose << "Use transparent: " << mTransparentColor << std::endl;
        
        core::Image entry = reader.ReadEntry(mEntryIndex);

        cfg.verbose << "Setting up palette" << std::endl;
        SetupPalette(entry, reader.Palette(mPaletteIndex));
        
        cfg.verbose << "Setting up format" << std::endl;
        SetupFormat(entry, gm1::PalettePixelFormat);

        cfg.verbose << "Setting up transparency" << std::endl;
        SetupTransparentColor(entry, mTransparentColor);

        const auto format = std::find_if(mFormats.begin(), mFormats.end(),
                                         [this](const RenderFormat &format) {
                                             return format.name == mFormat;
                                         });
        
        if(format != mFormats.end()) {            
            boost::filesystem::ofstream fout;
            if(!mEvalSizeOnly) {
                fout.open(mOutputFile, std::ios_base::binary | std::ios_base::out);
                if(!fout) {
                    throw std::runtime_error(strerror(errno));
                }
            }

            // Only for evaluating result size
            std::ostringstream dummy;

            std::ostream *out;
            if(mEvalSizeOnly) {
                out = &dummy;
            } else {
                out = &fout;
            }
            
            cfg.verbose << "Perform rendering" << std::endl;
            format->writer->Write(*out, entry);

            if(mEvalSizeOnly) {
                out->seekp(0, std::ios_base::end);
                cfg.stdout << out->tellp() << std::endl;
            }
            return EXIT_SUCCESS;
        } else {
            throw std::runtime_error("No format with such name");
        }
    }
}
