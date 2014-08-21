#include "listmode.h"

#include <sstream>
#include <vector>
#include <string>
#include <iostream>

#include <gmtool/table.h>
#include <gm1/gm1reader.h>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/positional_options.hpp>

#include "modes.h"

namespace po = boost::program_options;

namespace
{
    template<class T>
    std::vector<std::string> ToString(const std::vector<T> &xs)
    {
        std::ostringstream oss;
        std::vector<std::string> result(xs.size());
        for(size_t i = 0; i < xs.size(); ++i) {
            oss.str("");
            oss << xs[i];
            result[i] = oss.str();
        }
        return result;
    }
}

namespace gmtool
{
    void ListMode::GetOptions(boost::program_options::options_description &opts)
    {
        po::options_description mode("List mode");
        mode.add_options()
            (kFile, po::value(&mInputFile)->required(), "Set .gm1 filename")
            ;
        opts.add(mode);
    }

    void ListMode::GetPositionalOptions(boost::program_options::positional_options_description &unnamed)
    {
        unnamed.add(kFile, 1);
    }    

    int ListMode::Exec(const ModeConfig &cfg)
    {
        cfg.verbose << "Reading file " << mInputFile << std::endl;
        gm1::GM1Reader reader(mInputFile);
        cfg.verbose << reader.NumEntries() << " total" << std::endl;

        ShowEntryList(cfg.stdout, reader);

        return EXIT_SUCCESS;
    }

    std::vector<std::string> GetRow(int index, const gm1::EntryHeader &header)
    {
        return ToString<int> ({
                index,
                    header.width,
                    header.height,
                    header.posX,
                    header.posY,
                    header.group,
                    header.groupSize,
                    header.tileY,
                    header.tileOrient,
                    header.hOffset,
                    header.boxWidth,
                    header.flags
                    });
    }

    std::vector<std::pair<std::string, Alignment>> GetHeader()
    {
        return {
            {"#", Alignment::Right},
            {"Width", Alignment::Right},
            {"Height", Alignment::Right},
            {"PosX", Alignment::Right},
            {"PosY", Alignment::Right},
            {"Group", Alignment::Right},
            {"GroupSize", Alignment::Right},
            {"TileY", Alignment::Right},
            {"Orientation", Alignment::Left},
            {"Offset", Alignment::Right},
            {"BoxWidth", Alignment::Right},
            {"Flags", Alignment::Left}
        };
    }
    
    void ShowEntryList(std::ostream &out, const gm1::GM1Reader &reader)
    {
        Table table;
        for(const auto &h : GetHeader()) {
            table.AppendColumn(h.first, h.second);
        }
        for(size_t i = 0; i < reader.NumEntries(); ++i) {
            table.AppendRow(GetRow(i, reader.EntryHeader(i)));
        }

        table.PrintHeader(out);
        table.Print(out);
        table.PrintHeader(out);
    }
}
