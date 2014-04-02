#include "main.h"

#include "game/gm1reader.h"
#include "game/sdl_utils.h"
#include "game/surface.h"
#include "game/gm1.h"

#include "infomode.h"
#include "rendermode.h"
#include "dumpmode.h"

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
        : mVars()
        , mHandlers()
    {
        bpo::options_description base("Usage: gmtool <command> <archive> <entry> <options...>");
        base.add_options()
            ("help", "Show this text")
            ("verbose", bpo::value<bool>()->implicit_value(false), "Become verbose")
            ;

        bpo::options_description hidden;
        hidden.add_options()
            ("command", bpo::value<std::string>(), "dump, render or info")
            ("archive", bpo::value<std::string>(), "Source .gm1 archive")
            ("index", bpo::value<int>(), "Entry index in the archive")
            ;

        bpo::positional_options_description positional;
        positional.add("command", 1);
        positional.add("archive", 1);
        positional.add("index", 1);

        bpo::options_description visible;
        visible.add(base);
        RegisterModes();
        for(std::unique_ptr<ModeHandler> &ptr : mHandlers) {
            ptr->RegisterOptions(visible);
        }

        bpo::options_description all;
        all.add(visible);
        all.add(hidden);

        auto parsed = bpo::command_line_parser(argc, argv)
            .options(all)
            .positional(positional)
            .run();

        bpo::store(parsed, mVars);
        bpo::notify(mVars);

        if(mVars.count("help") != 0) {
            std::cout << visible << std::endl;
        }
    }

    void ToolMain::RegisterModes()
    {
        std::unique_ptr<ModeHandler> dump(new DumpMode);
        mHandlers.push_back(std::move(dump));
        
        std::unique_ptr<ModeHandler> info(new InfoMode);
        mHandlers.push_back(std::move(info));
    }
    
    int ToolMain::Exec()
    {
        if(mVars.count("command") == 0) {
            throw std::runtime_error("You should specify command!");
        }

        if(mVars.count("archive") == 0) {
            throw std::runtime_error("You should specify .gm1 file name!");
        }

        if(mVars.count("index") == 0) {
            throw std::runtime_error("You should specify entry index");
        }

        std::string command = mVars["command"].as<std::string>();

        for(std::unique_ptr<ModeHandler> &ptr : mHandlers) {
            if(ptr->ModeName() == command)
                return ptr->HandleMode(mVars);
        }

        // no mode with this name
        throw std::runtime_error("Unknown command");
    }
    
}
