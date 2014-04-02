#include "main.h"

#include "game/gm1reader.h"
#include "game/sdl_utils.h"
#include "game/surface.h"
#include "game/gm1.h"

#include <fstream>
#include <boost/program_options.hpp>

namespace bpo = boost::program_options;

int main(int argc, const char *argv[])
{
    bpo::options_description desc;
    desc.add_options()
        ("help", "this message")
        ("command", "followed execution mode");
    
    bpo::positional_options_description ps;
    ps.add("command", 1);
    
    bpo::variables_map vars;
    bpo::store(bpo::command_line_parser(argc, argv).options(desc).positional(ps).run(), vars);
    bpo::notify(vars);

    if(vars.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    int index = vars["index"].as<int>();
    std::string archivePath = vars["input"].as<std::string>();
    std::string dumpPath = vars["output"].as<std::string>();
    
    GM1::GM1Reader reader(archivePath);

    if(reader.NumEntries() <= index)
        throw std::runtime_error("No such index in the file");
    
    const char *data = reader.EntryData(index);

    if(vars.count("with-gm1-header")) {
        GM1::PrintHeader(std::cout, reader.Header());
    }

    if(vars.count("with-entry-header")) {
        GM1::PrintEntryHeader(std::cout, reader.EntryHeader(index));
    }

    std::ofstream fout(dumpPath, std::ios_base::binary);
    fout.write(data, reader.EntrySize(index));
    
    return 0;
}
