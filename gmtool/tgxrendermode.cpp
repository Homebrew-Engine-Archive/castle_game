#include "tgxrendermode.h"

#include "game/tgx.h"
#include "game/gm1.h"
#include "game/gm1reader.h"
#include "game/gm1entryreader.h"

#include <fstream>
#include <boost/program_options.hpp>
#include <string>
#include <vector>

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
        int index = vars["index"].as<int>();

        GM1::GM1Reader reader(archive);
        std::cout << archive << " has " << reader.NumEntries() << " entries" << std::endl;
            
        if(index >= reader.NumEntries()) {
            throw std::runtime_error("Index out of range");
        }

        // TODO need to convert palette

        uint32_t width = reader.Header().width;
        uint32_t height = reader.Header().height;
        
        std::ofstream fout(output, std::ios_base::binary);
        fout.write(reinterpret_cast<char*>(&width), sizeof(width));
        fout.write(reinterpret_cast<char*>(&height), sizeof(height));
        fout.write(reader.EntryData(index), reader.EntrySize(index));

        if(!fout) {
            std::ostringstream oss;
            oss << "Can't write image (" << errno << ")";
            throw std::runtime_error(oss.str());
        }
        
        return 0;
    }
    
}
