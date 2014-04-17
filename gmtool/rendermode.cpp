#include "rendermode.h"

#include <game/gm1.h>
#include <game/gm1reader.h>

#include <SDL2/SDL_image.h>

#include "colorvalidator.h"

namespace po = boost::program_options;

namespace GMTool
{
    void RenderMode::GetOptions(po::options_description &opts)
    {
        po::options_description mode("Render mode");
        mode.add_options()
            ("file", po::value(&mInputFile)->required(), "Set GM1 filename")
            ("index", po::value(&mEntryIndex)->required(), "Set entry index")
            ("format", po::value(&mFormat)->required(), "Set rendering format")
            ("tile-only", po::bool_switch(&mTileOnly), "Not render box texture")
            ("box-only", po::bool_switch(&mBoxOnly), "Not render tile texture")
            ("transparent-color", po::value(&mTransparentColor), "Background color")
            ("palette", po::value(&mPaletteIndex), "Set palette index for 8-bit entries")
            ;
        opts.add(mode);
    }
    
    void RenderMode::GetPositionalOptions(po::positional_options_description &unnamed)
    {
        unnamed.add("file", 1);
    }

    void RenderMode::PrintUsage(std::ostream &out)
    {
        out << "Allowed formats are:" << std::endl;
    }
    
    int RenderMode::Exec(const ModeConfig &cfg)
    {
        return EXIT_SUCCESS;
    }
}
