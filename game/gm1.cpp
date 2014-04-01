#include "gm1.h"

#include <stdexcept>
#include <string>
#include <iostream>
#include <sstream>
#include <boost/current_function.hpp>

namespace
{

    void Fail(const std::string &where, const std::string &what)
    {
        std::ostringstream oss;
        oss << where << " failed: " << what;
        throw std::runtime_error(oss.str());
    }

}

namespace GM1
{

    std::string GetImageClassName(uint32_t dataClass)
    {
        switch(dataClass) {
        case 1: return "Compressed 16 bit image";
        case 2: return "Compressed animation";
        case 3: return "Tile Object";
        case 4: return "Compressed font";
        case 5: return "Uncompressed bitmap";
        case 6: return "Compressed const size image";
        case 7: return "Uncompressed bitmap (other)";
        default: return "Unknown";
        }
    }
    
    GM1::Encoding GetEncoding(uint32_t dataClass)
    {
        switch(dataClass) {
        case 1: return Encoding::TGX16;
        case 2: return Encoding::TGX8;
        case 3: return Encoding::TileObject;
        case 4: return Encoding::Font;
        case 5: return Encoding::Bitmap;
        case 6: return Encoding::TGX16;
        case 7: return Encoding::Bitmap;
        default: return Encoding::Unknown;
        }
    }
    
    size_t GetPreambleSize(const GM1::Header &header)
    {
        size_t size = 0;

        /** About 88 bytes on GM1::Header **/
        size += GM1::CollectionHeaderBytes;

        /** About 10 palettes per file 512 bytes each **/
        size += GM1::CollectionPaletteCount * GM1::CollectionPaletteBytes;

        /** 32-bit size per entry **/
        size += header.imageCount * sizeof(uint32_t);

        /** 32-bit offset per entry **/
        size += header.imageCount * sizeof(uint32_t);
        
        /** Some GM1::EntryHeaders of 16 bytes long **/
        size += header.imageCount * GM1::CollectionEntryHeaderBytes;

        return size;
    }

    void PrintEntryHeader(std::ostream &out, const EntryHeader &header)
    {
        using namespace std;
        out << "Width: "            << static_cast<int>(header.width) << endl
            << "Height: "           << static_cast<int>(header.height) << endl
            << "PosX: "             << static_cast<int>(header.posX) << endl
            << "PosY: "             << static_cast<int>(header.posY) << endl
            << "Group: "            << static_cast<int>(header.group) << endl
            << "GroupSize: "        << static_cast<int>(header.groupSize) << endl
            << "TileY: "            << static_cast<int>(header.tileY) << endl
            << "TileOrient: "       << static_cast<int>(header.tileOrient) << endl
            << "Horiz Offset: "     << static_cast<int>(header.hOffset) << endl
            << "Box Width: "        << static_cast<int>(header.boxWidth) << endl
            << "Flags: "            << static_cast<int>(header.flags) << endl;
    }

    void PrintHeader(std::ostream &out, const Header &header)
    {
        using namespace std;
        out << "u1: "               << header.u1 << endl
            << "u2: "               << header.u2 << endl
            << "u3: "               << header.u3 << endl
            << "imageCount: "       << header.imageCount << endl
            << "u4: "               << header.u4 << endl
            << "dataClass: "        << GetImageClassName(header.dataClass) << endl
            << "u5: "               << header.u5 << endl
            << "u6: "               << header.u6 << endl
            << "sizeCategory: "     << header.sizeCategory << endl
            << "u7: "               << header.u7 << endl
            << "u8: "               << header.u8 << endl
            << "u9: "               << header.u9 << endl
            << "width: "            << header.width << endl
            << "height: "           << header.height << endl
            << "u10: "              << header.u10 << endl
            << "u11: "              << header.u11 << endl
            << "u12: "              << header.u12 << endl
            << "u13: "              << header.u13 << endl
            << "anchorX: "          << header.anchorX << endl
            << "anchorY: "          << header.anchorY << endl
            << "dataSize: "         << header.dataSize << endl
            << "u14: "              << header.u14 << endl;
    }

    void PrintPalette(std::ostream &out, const Palette &palette)
    {
        int column = 0;

        out << std::hex;
        for(auto color : palette) {
            column++;
            
            if(column % 16 == 0)
                out << std::endl;
            
            out << color << ' ';
        }

        out << std::endl;
    }

} // namespace GM1
