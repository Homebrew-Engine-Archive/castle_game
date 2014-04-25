#include "rendermode.h"

#include <string>
#include <sstream>
#include <memory>
#include <cstring>

#include <boost/filesystem/fstream.hpp>

#include <game/gm1.h>
#include <game/gm1reader.h>
#include <game/gm1palette.h>
#include <game/gm1entryreader.h>
#include <game/surface.h>
#include <game/sdl_utils.h>

#include <gmtool/rw.h>

namespace po = boost::program_options;

namespace GMTool
{
    RenderMode::RenderMode()
    {
        mFormats = RenderFormats();
    }
    
    void RenderMode::GetOptions(po::options_description &opts)
    {
        po::options_description mode("Render mode");
        mode.add_options()
            ("file", po::value(&mInputFile)->required(), "Set GM1 filename")
            ("index,i", po::value(&mEntryIndex)->required(), "Set entry index")
            ("output,o", po::value(&mOutputFile), "Set output image filename")
            ("format,f", po::value(&mFormat)->default_value(mFormats.front().name), "Set rendering format")
            ("palette,p", po::value(&mPaletteIndex), "Set palette index for 8-bit entries")
            ("transparent-color", po::value(&mTransparentColor)->default_value(DefaultTransparent()), "Set background color in ARGB format")
            ("approximate-size", po::bool_switch(&mApproxSize), "Prints size of resulting image in bytes")
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
        out << "Allowed formats are:" << std::endl;
        for(const RenderFormat &format : mFormats) {
            out.width(3);
            out << ' ';
            out << format.name;
            out << std::endl;
        }
    }

    SDL_Color RenderMode::DefaultTransparent() const
    {
        return MakeColor(240, 0, 255, 0);
    }
    
    void RenderMode::SetupPalette(Surface &surface, const GM1::Palette &palette)
    {
        if(HasPalette(surface)) {
            PalettePtr sdlPalette = GM1::CreateSDLPalette(palette);
            if(sdlPalette) {
                if(SDL_SetSurfacePalette(surface, sdlPalette.get()) < 0) {
                    throw std::runtime_error(SDL_GetError());
                }
            }
        }
    }

    void RenderMode::SetupFormat(Surface &surface, const PixelFormatPtr &format)
    {
        if(format->format != surface->format->format) {
            Surface tmp = SDL_ConvertSurface(surface, format.get(), NoFlags);
            if(!tmp) {
                throw std::runtime_error(SDL_GetError());
            }

            surface = tmp;
        }
    }

    void RenderMode::SetupTransparentColor(Surface &surface, const SDL_Color &color)
    {
        uint32_t colorkey = SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);
        if(SDL_SetColorKey(surface, SDL_TRUE, colorkey) < 0) {
            throw std::runtime_error(SDL_GetError());
        }
    }
    
    int RenderMode::Exec(const ModeConfig &cfg)
    {
        cfg.verbose << "Reading file " << mInputFile << std::endl;
        GM1::GM1Reader reader(mInputFile);

        cfg.verbose << "Collection contains " << reader.NumEntries() << " entries" << std::endl;

        if(mEntryIndex < 0 || mEntryIndex >= reader.NumEntries()) {
            throw std::logic_error("Entry index is out of range");
        }

        if(mPaletteIndex < 0 || mPaletteIndex >= reader.NumPalettes()) {
            throw std::logic_error("Palette index is out of range");
        }
        
        GM1::GM1EntryReader &entryReader = reader.EntryReader();
        if(DefaultTransparent() != mTransparentColor) {
            cfg.verbose << "Use transparent: " << mTransparentColor << std::endl;
            entryReader.Transparent(mTransparentColor);
        }
        
        Surface entry = entryReader.Load(reader, mEntryIndex);

        std::ostream *out = nullptr;

        std::ostringstream dummy;
        if(mApproxSize) {
            out = &dummy;
        }

        boost::filesystem::ofstream fout;
        if(!mApproxSize) {
            if(mOutputFile.empty()) {
                throw std::logic_error("You should specify --output option");
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
        SetupFormat(entry, GM1::PaletteFormat());

        cfg.verbose << "Setting up transparency" << std::endl;
        SetupTransparentColor(entry, mTransparentColor);

        const RenderFormat *result = nullptr;
        for(const RenderFormat &format : mFormats) {
            if(format.name == mFormat)
                result = &format;
        }

        if(result == nullptr) {
            throw std::logic_error("No format with such name");
        }

        result->renderer->RenderToStream(*out, entry);
        if(mApproxSize) {
            out->seekp(0, std::ios_base::end);
            cfg.stdout << out->tellp() << std::endl;
        }
        return EXIT_SUCCESS;
    }
}
