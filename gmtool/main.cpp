#include "main.h"

#include "../game/gm1reader.h"
#include "../game/sdl_utils.h"
#include "../game/surface.h"
#include "../game/gm1.h"

#include <fstream>
#include <boost/program_options.hpp>

namespace bpo = boost::program_options;

int main(int argc, const char *argv[])
{
    bpo::options_description desc;
    desc.add_options()
        ("help", "this message")
        ("output", bpo::value<std::string>()->default_value("data.out"), "specify output file")
        ("input", bpo::value<std::string>(), "*.gm1 file")
        ("index", bpo::value<int>(), "entry index")
        ("with-gm1-header", "prints gm1 header")
        ("with-entry-header", "prints entry header");

    bpo::positional_options_description ps;
    ps.add("input", 1);
    ps.add("index", 1);

    bpo::variables_map vars;
    bpo::store(bpo::command_line_parser(argc, argv).options(desc).positional(ps).run(), vars);
    bpo::notify(vars);

    if(vars.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    int index = vars["index"].as<int>();
    
    GM1::GM1Reader reader(vars["input"].as<std::string>());

    if(reader.NumEntries() <= index)
        throw std::runtime_error("No such index in the file");
    
    const char *data = reader.EntryData(index);

    if(vars.count("with-gm1-header")) {
        GM1::PrintHeader(std::cout, reader.Header());
    }

    if(vars.count("with-entry-header")) {
        GM1::PrintEntryHeader(std::cout, reader.EntryHeader(index));
    }

    std::ofstream fout(vars["output"].as<std::string>(), std::ios_base::binary);
    fout.write(data, reader.EntrySize(index));
    
    return 0;
}
