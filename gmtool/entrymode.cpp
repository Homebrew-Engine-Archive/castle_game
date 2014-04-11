#include "infomode.h"
#include "game/gm1.h"
#include "game/gm1reader.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <exception>

namespace bpo = boost::program_options;

namespace GMTool
{

    std::string InfoMode::ModeName() const
    {
        return "info";
    }
    
    void InfoMode::RegisterOptions(bpo::options_description &desc)
    {
        bpo::options_description infoOptions("Info mode options");
        infoOptions.add_options()
            ("without-header", "Hide GM1 header")
            ("without-entry-header", "Hide entry header")
            ("without-size", "Hide entry size")
            ("without-offset", "Hide entry offset")
            ;
        desc.add(infoOptions);
    }
    
    int InfoMode::HandleMode(const bpo::variables_map &vars)
    {
        std::string archive = vars["archive"].as<std::string>();
        int index = vars["index"].as<int>();

        GM1::GM1Reader reader(archive);

        std::cout << archive << " has " << reader.NumEntries() << " entries" << std::endl;
        if(index >= reader.NumEntries()) {
            throw std::runtime_error("Index out of range");
        }
        
        if(vars.count("without-header") == 0) {
            GM1::PrintHeader(std::cout, reader.Header());
        }

        if(vars.count("without-entry-header") == 0) {
            GM1::PrintEntryHeader(std::cout, reader.EntryHeader(index));
        }

        if(vars.count("without-size") == 0) {
            std::cout << "Entry size: " << reader.EntrySize(index) << std::endl;
        }

        if(vars.count("without-offset") == 0) {
            std::cout << "Entry offset: " << reader.EntryOffset(index) << std::endl;
        }

        return 0;
    }
    
}
