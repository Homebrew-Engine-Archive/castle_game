#include "main.h"

#include "game/gm1reader.h"
#include "game/sdl_utils.h"
#include "game/surface.h"
#include "game/gm1.h"

#include <sstream>
#include <memory>
#include <SDL.h>
#include <exception>
#include <stdexcept>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>

int main(int argc, const char *argv[])
{
    try {
        GMTool::ToolMain tool(argc, argv);
        return tool.Exec();
    } catch(const std::exception &error) {
        std::cerr << error.what() << std::endl;
        return -1;
    } catch(...) {
        std::cerr << "Unknown exception" << std::endl;
        return -1;
    }
}

namespace bpo = boost::program_options;

namespace GMTool
{

    ToolMain::ToolMain(int argc, const char *argv[])
        : vars()
    {
        bpo::options_description visible("Usage: gmtool <command> <archive> <entry> <options...>");
        visible.add_options()
            ("help", "Show this text")
            ("renderer", bpo::value<std::string>()->default_value("tgx"), "Can be tgx or png")
            ("output,o", bpo::value<std::string>()->default_value("dump.out"), "Dump filename")
            ("palette-index", bpo::value<int>()->default_value(0), "Palette index to use when render 8-bit image")
            ("without-header", "Hide GM1 header")
            ("without-entry-header", "Hide entry header")
            ("without-size", "Hide entry size")
            ("without-offset", "Hide entry offset")
            ;

        bpo::options_description hidden;
        hidden.add_options()
            ("command", bpo::value<std::string>(), "Can be dump, render or info")
            ("archive", bpo::value<std::string>(), "Source .gm1 archive")
            ("index", bpo::value<int>(), "Entry index in the archive")
            ;

        bpo::positional_options_description positional;
        positional.add("command", 1);
        positional.add("archive", 1);
        positional.add("index", 1);

        bpo::options_description options;
        options.add(visible);
        options.add(hidden);

        auto parsed = bpo::command_line_parser(argc, argv)
            .options(options)
            .positional(positional)
            .run();

        bpo::store(parsed, vars);
        bpo::notify(vars);

        if(vars.count("help") != 0) {
            std::cout << options << std::endl;
        }
    }

    int ToolMain::Exec()
    {
        if(vars.count("command") == 0) {
            throw std::runtime_error("You should specify command!");
        }

        if(vars.count("archive") == 0) {
            throw std::runtime_error("You should specify .gm1 file name!");
        }

        if(vars.count("index") == 0) {
            throw std::runtime_error("You should specify entry index");
        }

        std::string command = vars["command"].as<std::string>();
        if(command == "dump") {
            return Dump();
        } else if(command == "render") {
            return Render();
        } else if(command == "info") {
            return Info();
        } else {
            throw std::runtime_error("Unknown command");
        }
    }

    int ToolMain::Dump()
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

    int ToolMain::Info()
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

    int ToolMain::Render()
    {
        throw std::runtime_error("Not implemented");
    }
    
}
