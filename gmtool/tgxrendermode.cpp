#include "tgxrendermode.h"

#include <game/tgx.h>
#include <game/gm1.h>
#include <game/gm1reader.h>
#include <game/gm1entryreader.h>
#include <game/gm1palette.h>
#include <game/endianness.h>
#include <game/surface.h>

#include <fstream>
#include <string>
#include <vector>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/program_options.hpp>

namespace bpo = boost::program_options;

namespace GMTool
{
    void TGXRenderMode::RegisterOptions(bpo::options_description &desc)
    {
        
    }
    
    std::string TGXRenderMode::ModeName() const
    {
        return std::string("tgx");
    }
    
    int TGXRenderMode::HandleMode(const bpo::variables_map &vars)
    {
        std::string archive = vars["archive"].as<std::string>();
        std::string output = vars["output"].as<std::string>();
        int entryIndex = vars["index"].as<int>();
        int paletteIndex = vars["palette"].as<int>();

        GM1::GM1Reader reader(archive);
        std::cout << archive << " has " << reader.NumEntries() << " entries" << std::endl;
            
        if(entryIndex >= reader.NumEntries()) {
            throw std::runtime_error("Entry index out of range");
        }

        if(paletteIndex >= reader.NumPalettes()) {
            throw std::runtime_error("Palette index out of range");
        }

        const GM1::GM1EntryReader *entryReader = reader.EntryReader();
        Surface surface = entryReader->Load(reader, entryIndex);

        if(entryReader->Palettized()) {
            PixelFormatPtr &&format = GM1::GetPaletteFormat();
            PalettePtr &&palette = GM1::CreateSDLPalette(
                reader.Palette(paletteIndex));
            if(SDL_SetSurfacePalette(surface, palette.get()) < 0) {
                std::ostringstream oss;
                oss << "In " << __FILE__ << " at " << __LINE__ << ": ";
                oss << SDL_GetError();
                throw std::runtime_error(oss.str());
            }
            Surface converted = SDL_ConvertSurface(surface, format.get(), NoFlags);
            if(converted.Null()) {
                std::ostringstream oss;
                oss << "In " << __FILE__ << " at " << __LINE__ << ": ";
                oss << SDL_GetError();
                throw std::runtime_error(oss.str());
            }
            surface = converted;
        }

        std::ofstream fout(output, std::ios_base::binary);
        TGX::WriteSurface(fout, surface);

        if(!fout) {
            std::ostringstream oss;
            oss << "Can't write image (" << errno << ")";
            throw std::runtime_error(oss.str());
        }
        
        return 0;
    }
    
}
