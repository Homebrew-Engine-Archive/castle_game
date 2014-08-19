#include "engine.h"

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

#include <gm1/gm1.h>
#include <gm1/gm1reader.h>

#include <core/sdl_utils.h>

#include <gmtool/table.h>
#include <gmtool/headermode.h>
#include <gmtool/palettemode.h>
#include <gmtool/listmode.h>
#include <gmtool/entrymode.h>
#include <gmtool/dumpmode.h>
#include <gmtool/rendermode.h>
#include <gmtool/unpackmode.h>
#include <gmtool/packmode.h>

namespace po = boost::program_options;

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

        Table table;
        table.AppendColumn("Command", Alignment::Left);
        table.AppendColumn("Description", Alignment::Left);
        for(const Command &command : commands) {
            table.AppendRow({command.name, command.description});
        }
        table.Print(out);
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
        
    std::vector<Command> GetCommandList()
    {
        return {
            {"header",  "Show gm1 header",                     Mode::Ptr(new HeaderMode)},
            {"palette", "Show gm1 palettes",                   Mode::Ptr(new PaletteMode)},
            {"entry",   "Show header of gm1 entries",          Mode::Ptr(new EntryMode)},
            {"list",    "List entries of gm1 collection",      Mode::Ptr(new ListMode)},
            {"dump",    "Dump entry data onto stdout",         Mode::Ptr(new DumpMode)},
            {"render",  "Convert entry into trivial image",    Mode::Ptr(new RenderMode)},
            {"unpack",  "Unpack gm1 collection",               Mode::Ptr(new UnpackMode)},
            {"pack",    "Pack directory into gm1",             Mode::Ptr(new PackMode)}
        };
    }

    int Engine::Exec(int argc, const char *argv[])
    {
        po::options_description visible("Allowed options");
        visible.add_options()
            ("help,h", po::bool_switch(&mHelpRequested), "produce help message")
            ("version", po::bool_switch(&mVersionRequested), "show version")
            ("verbose,v", po::bool_switch(&mVerboseRequested), "allow verbose messages")
            ;

        std::vector<std::string> extras;
        po::options_description overall;
        overall.add(visible);
        overall.add_options()
            ("mode", po::value(&mModeName))
            ("extras", po::value(&extras))
            ;

        po::positional_options_description unnamed;
        unnamed.add("mode", 1);

        // We don't need these options there. What we really need is to forward
        // them to command's parser, but boost can't just ignore unrecognized
        // positional options for us. So we parse them as "extra" options and
        // append to the rest.
        // Tested with BOOST_LIB_VERSION "1_54"
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

        if(mVersionRequested) {
            ShowVersion(std::cout);
            return EXIT_SUCCESS;
        }

        // Here we merge unrecognized positional and non-positional options.
        // Further we can parse it again using more appreciate command parser.
        std::copy(extras.begin(), extras.end(), std::back_inserter(unparsed));

        const std::vector<Command> &commands = GetCommandList();
        
        if(mModeName.empty()) {
            if(mHelpRequested) {
                ShowUsage(std::cout);
                std::cout << visible << std::endl;
                ShowCommandList(std::cout, commands);
                return EXIT_SUCCESS;
            } else {
                throw std::runtime_error("Command required but missing");
            }
        }

        const auto command = std::find_if(commands.begin(), commands.end(),
                                          [this](const Command &command) {
                                              return mModeName == command.name;
                                          });

        if(command != commands.end()) {
            po::options_description opts;
            command->mode->GetOptions(opts);

            if(mHelpRequested) {
                std::cout << opts << std::endl;
                command->mode->PrintUsage(std::cout);
                return EXIT_SUCCESS;
            }

            po::positional_options_description unnamed;
            command->mode->GetPositionalOptions(unnamed);

            po::parsed_options parsed = po::command_line_parser(unparsed)
                .options(opts)
                .positional(unnamed)
                .run();

            po::variables_map vars;
            po::store(parsed, vars);
            po::notify(vars);

            // Dummy stream for dull verbosity.
            std::ostream null(nullptr);
            std::ostream &verbose = (mVerboseRequested ? std::clog : null);
            return command->mode->Exec({verbose, std::cout});
        } else {
            throw std::runtime_error("No command with such name");
        }
    }
}
