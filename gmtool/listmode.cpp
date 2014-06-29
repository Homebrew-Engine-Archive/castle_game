#include "listmode.h"

#include <sstream>
#include <vector>
#include <string>
#include <iostream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/positional_options.hpp>

#include <gm1/gm1reader.h>

namespace po = boost::program_options;

namespace
{
    std::ostringstream dummy;
    
    std::ostream& LeftPadded(std::ostream &out, int padding)
    {
        out.width(padding);
        out << std::right;
        return out;
    }
        
    template<class T> std::string ToString(T t)
    {
        dummy.str("");
        dummy << t;
        return dummy.str();
    }
    
    std::vector<std::string> GetEntryCells(int index, const gm1::EntryHeader &header)
    {
        using namespace std;
        return vector<string>
        {
            ToString<int>(index),
            ToString<int>(header.width),
            ToString<int>(header.height),
            ToString<int>(header.posX),
            ToString<int>(header.posY),
            ToString<int>(header.group),
            ToString<int>(header.groupSize),
            ToString<int>(header.tileY),
            ToString<int>(header.tileOrient),
            ToString<int>(header.hOffset),
            ToString<int>(header.boxWidth),
            ToString<int>(header.flags)
        };
    }

    std::vector<std::string> GetHeadings()
    {
        return std::vector<std::string>
        {
            "###",
            "Width",
            "Height",
            "PosX",
            "PosY",
            "Group",
            "GroupSize",
            "TileY",
            "Orientation",
            "Offset",
            "BoxWidth",
            "Flags"
        };
    }
}

namespace gmtool
{
    void ListMode::GetOptions(boost::program_options::options_description &opts)
    {
        po::options_description mode("List mode");
        mode.add_options()
            ("file", po::value(&mInputFile)->required(), "Set .gm1 filename")
            ;
        opts.add(mode);
    }
    
    void ListMode::GetPositionalOptions(boost::program_options::positional_options_description &unnamed)
    {
        unnamed.add("file", 1);
    }
    
    int ListMode::Exec(const ModeConfig &cfg)
    {
        cfg.verbose << "Reading file " << mInputFile << std::endl;
        gm1::GM1Reader reader(mInputFile);

        ShowEntryList(cfg.stdout, reader);
        cfg.verbose << reader.NumEntries() << " total" << std::endl;
        
        return EXIT_SUCCESS;
    }
    
    void ShowEntryList(std::ostream &out, const gm1::GM1Reader &reader)
    {
        std::vector<std::vector<std::string>> cells;
        cells.reserve(reader.NumEntries() + 1 /** header **/);

        std::vector<std::string> heading = GetHeadings();
        std::vector<size_t> width(heading.size(), 0);
        cells.emplace_back(std::move(heading));

        for(size_t index = 0; index < reader.NumEntries(); ++index) {
            cells.emplace_back(
                std::move(
                    GetEntryCells(
                        index, reader.EntryHeader(index))));
        }

        for(const auto &row : cells) {
            for(size_t i = 0; i < width.size(); ++i) {
                width[i] = std::max(width[i], row[i].length());
            }
        }

        for(const auto &row : cells) {
            for(size_t i = 0; i < width.size(); ++i) {
                LeftPadded(out, width[i] + 1) << row[i];
            }
            out << std::endl;
        }
    }
}
