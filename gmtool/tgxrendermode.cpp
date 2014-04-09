#include "tgxrendermode.h"

#include <game/tgx.h>
#include <game/gm1.h>
#include <game/gm1reader.h>
#include <game/gm1entryreader.h>
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
            throw std::runtime_error("Index out of range");
        }
        
        const GM1::GM1EntryReader *er = reader.EntryReader();

        Surface surface = er->Load(reader, entryIndex);

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
