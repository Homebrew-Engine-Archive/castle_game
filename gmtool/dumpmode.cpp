#include "dumpmode.h"

#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <boost/program_options.hpp>

#include "game/gm1reader.h"
#include "game/gm1.h"

namespace bpo = boost::program_options;

namespace GMTool
{

    std::string DumpMode::ModeName() const   
    {
        return "dump";
    }
    
    void DumpMode::RegisterOptions(bpo::options_description &desc)
    {
        bpo::options_description dumpOptions("Dump mode options");
        dumpOptions.add_options()
            ("dumpfile", bpo::value<std::string>()->default_value("dump.out"), "Dump filename")
            ;

        desc.add(dumpOptions);
    }

    int DumpMode::HandleMode(const bpo::variables_map &vars)
    {
        std::string output = vars["output"].as<std::string>();
        std::string archive = vars["archive"].as<std::string>();
        int index = vars["index"].as<int>();

        GM1::GM1Reader reader(archive);
        if(index >= reader.NumEntries()) {
            throw std::runtime_error("Index out of range");
        }

        std::ofstream fout(output, std::ios_base::binary);
        fout.write(reader.EntryData(index), reader.EntrySize(index));

        if(!fout) {
            std::ostringstream oss;
            oss << "Write dump failed (" << errno << ")";
            throw std::runtime_error(oss.str());
        }
        fout.close();

        return 0;
    }
    
}
