#include "main.h"

#include "config_gmtool.h"

#include <cstdlib>
#include <cstdint>

#include <algorithm>
#include <sstream>
#include <stdexcept>

#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/filesystem/path.hpp>

#include <gm1/gm1.h>
#include <gm1/gm1reader.h>

#include <core/sdl_utils.h>

#include <gmtool/table_formatter.h>
#include <gmtool/headermode.h>
#include <gmtool/palettemode.h>
#include <gmtool/listmode.h>
#include <gmtool/entrymode.h>
#include <gmtool/dumpmode.h>
#include <gmtool/rendermode.h>
#include <gmtool/unpackmode.h>
#include <gmtool/packmode.h>

int main(int argc, const char *argv[])
{
    try {
        gmtool::ToolMain tool;
        return tool.Exec(argc, argv);
    } catch(const std::exception &error) {
        std::cerr << error.what() << std::endl;
        return EXIT_FAILURE;
    }
}

namespace gmtool
{
    struct Command
    {
        std::string name;
        std::string description;
        Mode::Ptr mode;
    };
    
    void ShowCommandList(std::ostream &out, const std::vector<Command> &commands)
    {
        out << "Allowed commands: " << std::endl;

        int maxLength = 0;
        for(const Command &command : commands) {
            if(command.mode) {
                maxLength = std::max<int>(maxLength, command.name.length());
            }
        }
        
        for(const Command &command : commands) {
            if(command.mode) {
                PrintColumn(out, command.name, maxLength);
                PrintColumn(out, command.description);
                out << std::endl;
            }
        }
    }

    void ShowVersion(std::ostream &out)
    {
        out << GMTOOL_MAJOR << '.' << GMTOOL_MINOR << std::endl;
    }

    void ShowUsage(std::ostream &out)
    {
        out << "Usage: ./gmtool <command> <options...>" << std::endl;
        out << "Type `./gmtool --help <command>` to get precise help message" << std::endl;
        out << std::endl;
    }
}

namespace po = boost::program_options;

namespace gmtool
{
    std::vector<Command> ToolMain::commandList = {
        {"header",  "Show gm1 header",                     Mode::Ptr(new HeaderMode)},
        {"palette", "Show gm1 palettes",                   Mode::Ptr(new PaletteMode)},
        {"entry",   "Show header of gm1 entries",          Mode::Ptr(new EntryMode)},
        {"list",    "List entries of gm1 collection",      Mode::Ptr(new ListMode)},
        {"dump",    "Dump entry data onto stdout",         Mode::Ptr(new DumpMode)},
        {"render",  "Convert entry into trivial image",    Mode::Ptr(new RenderMode)},
        {"unpack",  "Unpack gm1 collection",               Mode::Ptr(new UnpackMode)},
        {"pack",    "Pack directory into gm1",             Mode::Ptr(new PackMode)}
    };

    int ToolMain::Exec(int argc, const char *argv[])
    {
        std::vector<Command> commands = this->commandList;

        bool helpRequested = false;
        bool versionRequested = false;
        bool allowVerbose = false;
        bool noUnusedBytes = false;
        std::string modeName;

        po::options_description visible("Allowed options");
        visible.add_options()
            ("help,h", po::bool_switch(&helpRequested), "produce help message")
            ("version", po::bool_switch(&versionRequested), "show version")
            ("verbose,v", po::bool_switch(&allowVerbose), "allow verbose messages")
            ("no-unused-bytes", po::bool_switch(&noUnusedBytes), "report any unused bytes in file")
            ;

        std::vector<std::string> extras;
        po::options_description overall;
        overall.add(visible);
        overall.add_options()
            ("mode", po::value(&modeName))
            ("extras", po::value(&extras))
            ;

        po::positional_options_description unnamed;
        unnamed.add("mode", 1);

        /// We are no really need it here, but boost can't just ignore
        /// unrecognized positional options for us. So we put it into additional
        /// vector and then append to the rest.
        unnamed.add("extras", -1);

        po::parsed_options parsed = po::command_line_parser(argc, argv)
            .options(overall)
            .positional(unnamed)
            .allow_unregistered()
            .run();

        std::vector<std::string> unparsed = po::collect_unrecognized(parsed.options, po::exclude_positional);

        po::variables_map vars;
        po::store(parsed, vars);
        po::notify(vars);

        /// Here we merge unrecognized positional and non-positional options.
        /// Further we can parse it again using more appreciate command parser.
        std::copy(extras.begin(), extras.end(), std::back_inserter(unparsed));

        if(versionRequested) {
            ShowVersion(std::cout);
            return EXIT_SUCCESS;
        }

        if(modeName.empty()) {
            if(helpRequested) {
                ShowUsage(std::cout);
                std::cout << visible << std::endl;
                ShowCommandList(std::cout, commands);
                return EXIT_SUCCESS;
            }
            throw std::runtime_error("Command required but missing");
        }

        /// Dummy stream for dull verbosity.
        std::ostream null(nullptr);
        std::ostream &verbose = (allowVerbose ? std::clog : null);
        
        ModeConfig config {helpRequested, versionRequested, allowVerbose, verbose, std::cout};

        for(const Command &lookup : commands) {
            if(lookup.name == modeName) {
                return RunCommand(unparsed, lookup, config);
            }
        }

        throw std::runtime_error("No command with such name");
    }

    int ToolMain::RunCommand(const std::vector<std::string> &args, const Command &command, const ModeConfig &cfg)
    {
        po::options_description opts;
        command.mode->GetOptions(opts);

        if(cfg.helpRequested) {
            std::cout << opts << std::endl;
            command.mode->PrintUsage(std::cout);
            return EXIT_SUCCESS;
        }

        po::positional_options_description unnamed;
        command.mode->GetPositionalOptions(unnamed);

        po::parsed_options parsed = po::command_line_parser(args)
            .options(opts)
            .positional(unnamed)
            .run();

        po::variables_map vars;
        po::store(parsed, vars);
        po::notify(vars);

        return command.mode->Exec(cfg);
    }
}
