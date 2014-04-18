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
            ("output,o", po::value(&mOutputFile)->required(), "Set output image filename")
            ("index,i", po::value(&mEntryIndex), "Set entry index")
            ("all", po::bool_switch(&mRenderAll), "Render all entries")
            ("format,f", po::value(&mFormat)->default_value(mFormats.front().name), "Set rendering format")
            ("palette,p", po::value(&mPaletteIndex), "Set palette index for 8-bit entries")
            ("transparent-color", po::value(&mTransparentColor)->default_value(HexColor{0x00ff00ff}), "Set background color")
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

    boost::filesystem::path RenderMode::GetOutputName(int index)
    {
        boost::filesystem::path temp;
        temp += mOutputFile.parent_path();
        temp /= mOutputFile.stem();
        temp += std::to_string(index);
        temp += mOutputFile.extension();
        return temp;
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
        if(format->BitsPerPixel != surface->format->BitsPerPixel) {
            surface = SDL_ConvertSurface(surface, format.get(), NoFlags);
            if(!surface) {
                throw std::runtime_error(SDL_GetError());
            }
        }
    }

    int RenderMode::Exec(const ModeConfig &cfg)
    {
        cfg.verbose << "Reading file " << mInputFile << std::endl;
        GM1::GM1Reader reader(mInputFile);

        cfg.verbose << "Collection contains " << reader.NumEntries() << " entries" << std::endl;

        std::vector<int> indices;
        if(mRenderAll) {
            indices.resize(reader.NumEntries());
            std::iota(indices.begin(), indices.end(), 0);
        } else {
            if(mEntryIndex < 0 || mEntryIndex >= reader.NumEntries()) {
                throw std::logic_error("Entry index is out of range");
            }
            indices.push_back(mEntryIndex);
        }

        if(mPaletteIndex < 0 || mPaletteIndex >= reader.NumPalettes()) {
            throw std::logic_error("Palette index is out of range");
        }

        const GM1::GM1EntryReader &entryReader = reader.EntryReader();

        RenderFormat format;
        {
            auto searchByName = [this](const RenderFormat &format) {
                return format.name == mFormat;
            };
            auto searchResult = std::find_if(mFormats.begin(), mFormats.end(), searchByName);
            if(searchResult == mFormats.end()) {
                throw std::logic_error("No format with such name");
            } else {
                format = *searchResult;
            }
        }
        
        for(int index : indices) {
            Surface entry = entryReader.Load(reader, index);

            boost::filesystem::path outputName;
            if(mRenderAll) {
                outputName = GetOutputName(index);
                cfg.verbose << "Rendering entry " << index << " in " << outputName << std::endl;
            } else {
                outputName = mOutputFile;
            }
            
            boost::filesystem::ofstream fout(outputName, std::ios_base::binary | std::ios_base::out);
            if(!fout) {
                throw std::runtime_error(strerror(errno));
            }
            
            cfg.verbose << "Setting up palette" << std::endl;
            SetupPalette(entry, reader.Palette(mPaletteIndex));
            
            cfg.verbose << "Setting up format" << std::endl;
            SetupFormat(entry, GM1::PaletteFormat());
            
            format.renderer->RenderToStream(fout, entry);
        }
            
        return EXIT_SUCCESS;
    }
}
