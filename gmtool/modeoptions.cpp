#include "headermode.h"
#include "palettemode.h"
#include "rendermode.h"
#include "listmode.h"
#include "dumpmode.h"
#include "entrymode.h"
#include "packmode.h"
#include "unpackmode.h"

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/positional_options.hpp>

namespace po = boost::program_options;

namespace
{
    // Common string literals for option names
    const char *kFile = "file";
    const char *kIndex = "index,i";
    const char *kPalette = "palette,p";
    const char *kTilePart = "tile-part";
    const char *kBinary = "binary";
    const char *kCount = "count";
    const char *kType = "type";
    const char *kFormat = "format,f";
    const char *kTransparentColor = "transparent-color";
}

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

    void PaletteMode::GetOptions(po::options_description &opts)
    {
        po::options_description mode("Palette mode");
        mode.add_options()
            (kFile,     po::value(&mInputFile)->required(), "Set .gm1 filename")
            (kPalette,  po::value(&mPaletteIndex),          "Set palette index")
            (kBinary,   po::bool_switch(&mBinary),          "Dump palette in binary")
            (kCount,    po::bool_switch(&mCountRequested),  "Print palettes count")
            ;
        opts.add(mode);
    }

    void PaletteMode::GetPositionalOptions(po::positional_options_description &unnamed)
    {
        unnamed.add(kFile, 1);
    }

    void RenderMode::GetOptions(po::options_description &opts)
    {
        po::options_description mode("Render mode");
        mode.add_options()
            (kFile,               po::value(&mInputFile)->required(),                                    "Set GM1 filename")
            (kIndex,              po::value(&mEntryIndex)->required(),                                   "Set entry index")
            (kFormat,             po::value(&mFormat)->default_value(mFormats.front().name),             "Set render file format")
            (kPalette,            po::value(&mPaletteIndex),                                             "Set palette index for 8-bit entries")
            (kTransparentColor,   po::value(&mTransparentColor)->default_value(DefaultTransparent()),    "Set background color in #AARRGGBB format")
            (kTilePart,           po::value(&mTilePart)->default_value(TilePart::Both),                  "What part of tile will be rendered (tile, box or both)")
            ;
        opts.add(mode);
    }
    
    void RenderMode::GetPositionalOptions(po::positional_options_description &unnamed)
    {
        unnamed.add(kFile, 1);
    }

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
    
    void ListMode::GetOptions(boost::program_options::options_description &opts)
    {
        po::options_description mode("List mode");
        mode.add_options()
            (kFile, po::value(&mInputFile)->required(), "Set .gm1 filename")
            ;
        opts.add(mode);
    }

    void ListMode::GetPositionalOptions(boost::program_options::positional_options_description &unnamed)
    {
        unnamed.add(kFile, 1);
    }    

    void DumpMode::GetOptions(boost::program_options::options_description &opts)
    {
        po::options_description mode("Dump mode");
        mode.add_options()
            (kFile, po::value(&mInputFile)->required(), "Set .gm1 filename")
            (kIndex, po::value(&mEntryIndex)->required(), "Set entry index")
            (kTilePart, po::value(&mTilePart)->default_value(TilePart::Both), "What part of tile will be dumped (tile, box or both)")
            ;
        opts.add(mode);
    }
    
    void DumpMode::GetPositionalOptions(boost::program_options::positional_options_description &unnamed)
    {
        unnamed.add(kFile, 1);
    }    
}
