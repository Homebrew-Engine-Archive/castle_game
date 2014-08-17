#include "rendermode.h"

#include <string>
#include <sstream>
#include <memory>
#include <cstring>

#include <boost/filesystem/fstream.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/positional_options.hpp>

#include <gmtool/renderer.h>
#include <gmtool/table.h>

#include <gm1/gm1.h>
#include <gm1/gm1reader.h>
#include <gm1/gm1entryreader.h>

#include <core/image.h>
#include <core/palette.h>
#include <core/color.h>
#include <core/rect.h>
#include <core/rw.h>

namespace po = boost::program_options;

namespace gmtool
{
    RenderMode::~RenderMode() throw() = default;
    RenderMode::RenderMode()
    {
        mFormats = RenderFormats();
    }
    
    void RenderMode::GetOptions(po::options_description &opts)
    {
        po::options_description mode("Render mode");
        mode.add_options()
            ("file",              po::value(&mInputFile)->required(),                                    "Set GM1 filename")
            ("index,i",           po::value(&mEntryIndex)->required(),                                   "Set entry index")
            ("output,o",          po::value(&mOutputFile),                                               "Set output image filename")
            ("format,f",          po::value(&mFormat)->default_value(mFormats.front().name),             "Set render file format")
            ("palette,p",         po::value(&mPaletteIndex),                                             "Set palette index for 8-bit entries")
            ("transparent-color", po::value(&mTransparentColor)->default_value(DefaultTransparent()),    "Set background color in #AARRGGBB format")
            ("print-size-only",   po::bool_switch(&mEvalSizeOnly),                                       "Do not perform real rendering, but eval and print size")
            ("tile-only",         po::bool_switch(&mRenderTileOnly),                                     "Render only 30x16 tile rhombus if appreciated")
            ;
        opts.add(mode);
    }
    
    void RenderMode::GetPositionalOptions(po::positional_options_description &unnamed)
    {
        unnamed.add("file", 1);
        unnamed.add("output", 1);
    }

    void RenderMode::PrintUsage(std::ostream &out)
    {
        out << "Allowed render formats are:" << std::endl;

        Table table;
        table.AppendColumn("Format", Alignment::Left);
        for(const RenderFormat &format : mFormats) {
            table.AppendRow({format.name});
        }
        out << table;
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
        if(mRenderTileOnly) {
            cfg.verbose << "Set TileOnly flag ON" << std::endl;
            flags |= gm1::GM1Reader::TileOnly;
        } else {
            cfg.verbose << "Set TileOnly flag OFF" << std::endl;
        }
        
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

        boost::filesystem::ofstream fout;
        std::ostringstream dummy;
        
        std::ostream *out = nullptr;

        if(mEvalSizeOnly) {
            out = &dummy;
        } else {
            if(mOutputFile.empty()) {
                throw std::runtime_error("You should specify --output option");
            }
            fout.open(mOutputFile, std::ios_base::binary | std::ios_base::out);
            if(!fout) {
                throw std::runtime_error(strerror(errno));
            }
            out = &fout;
        }
        
        cfg.verbose << "Setting up palette" << std::endl;
        SetupPalette(entry, reader.Palette(mPaletteIndex));
        
        cfg.verbose << "Setting up format" << std::endl;
        SetupFormat(entry, gm1::PalettePixelFormat);

        cfg.verbose << "Setting up transparency" << std::endl;
        SetupTransparentColor(entry, mTransparentColor);

        cfg.verbose << "Find appropriate format" << std::endl;
        const RenderFormat *format = nullptr;
        for(const RenderFormat &candidate : mFormats) {
            if(candidate.name == mFormat)
                format = &candidate;
        }

        if(format == nullptr) {
            throw std::runtime_error("No format with such name");
        }

        cfg.verbose << "Perform rendering" << std::endl;
        format->renderer->RenderToStream(*out, entry);

        if(mEvalSizeOnly) {
            out->seekp(0, std::ios_base::end);
            cfg.stdout << out->tellp() << std::endl;
        }
        return EXIT_SUCCESS;
    }
}
