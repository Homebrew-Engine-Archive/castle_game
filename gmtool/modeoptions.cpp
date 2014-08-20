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

namespace gmtool
{
    void HeaderMode::GetOptions(po::options_description &opts)
    {
        po::options_description mode("Header mode");
        mode.add_options()
            ("file", po::value(&mInputFile)->required(), "Set .gm1 filename")
            ("binary", po::bool_switch(&mBinary), "Print header in binary format")
            ("count", po::bool_switch(&mCountRequested), "Print entries count")
            ("type", po::bool_switch(&mReaderTypeRequested), "Print prefered reader's type")
            ;
        opts.add(mode);
    }

    void HeaderMode::GetPositionalOptions(po::positional_options_description &unnamed)
    {
        unnamed.add("file", 1);
    }

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

    void RenderMode::GetOptions(po::options_description &opts)
    {
        po::options_description mode("Render mode");
        mode.add_options()
            ("file",              po::value(&mInputFile)->required(),                                    "Set GM1 filename")
            ("index,i",           po::value(&mEntryIndex)->required(),                                   "Set entry index")
            ("format,f",          po::value(&mFormat)->default_value(mFormats.front().name),             "Set render file format")
            ("palette,p",         po::value(&mPaletteIndex),                                             "Set palette index for 8-bit entries")
            ("transparent-color", po::value(&mTransparentColor)->default_value(DefaultTransparent()),    "Set background color in #AARRGGBB format")
            ("tile-part",         po::value(&mTilePart)->default_value(TilePart::Both),                  "What part of tile will be rendered (tile, box or both)")
            ;
        opts.add(mode);
    }
    
    void RenderMode::GetPositionalOptions(po::positional_options_description &unnamed)
    {
        unnamed.add("file", 1);
    }

    void EntryMode::GetOptions(po::options_description &opts)
    {
        po::options_description mode("Entry mode");
        mode.add_options()
            ("file", po::value(&mInputFile)->required(), "Set .gm1 filename")
            ("index", po::value(&mEntryIndex)->required(), "Set entry index")
            ("binary", po::bool_switch(&mBinary), "Dump entry in binary")
            ;
        opts.add(mode);
    }

    void EntryMode::GetPositionalOptions(po::positional_options_description &unnamed)
    {
        unnamed.add("file", 1);
        unnamed.add("index", 1);
    }
    
    void ListMode::GetOptions(boost::program_options::options_description &opts)
    {
        po::options_description mode("List mode");
        mode.add_options()
            ("file", po::value(&mInputFile)->required(), "Set .gm1 filename")
            ;
        opts.add(mode);
    }

    void ListMode::GetPositionalOptions(boost::program_options::positional_options_description &unnamed)
    {
        unnamed.add("file", 1);
    }    

    void DumpMode::GetOptions(boost::program_options::options_description &opts)
    {
        po::options_description mode("Dump mode");
        mode.add_options()
            ("file", po::value(&mInputFile)->required(), "Set .gm1 filename")
            ("i,index", po::value(&mEntryIndex)->required(), "Set entry index")
            ("tile-part", po::value(&mTilePart)->default_value(TilePart::Both), "What part of tile will be dumped (tile, box or both)")
            ;
        opts.add(mode);
    }
    
    void DumpMode::GetPositionalOptions(boost::program_options::positional_options_description &unnamed)
    {
        unnamed.add("file", 1);
    }    
}
