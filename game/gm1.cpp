#include "gm1.h"

#include <string>
#include <iostream>
#include <sstream>

#include <core/iohelpers.h>

#include <game/palette.h>

namespace gm1
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
    
    gm1::ArchiveType GetArchiveType(uint32_t dataClass)
    {
        switch(dataClass) {
        case 1: return ArchiveType::TGX16;
        case 2: return ArchiveType::TGX8;
        case 3: return ArchiveType::TileObject;
        case 4: return ArchiveType::Font;
        case 5: return ArchiveType::Bitmap;
        case 6: return ArchiveType::TGX16;
        case 7: return ArchiveType::Bitmap;
        default: return ArchiveType::Unknown;
        }
    }

    std::string GetArchiveTypeName(gm1::ArchiveType encoding)
    {
        switch(encoding) {
        case gm1::ArchiveType::TGX16: return "TGX16";
        case gm1::ArchiveType::TGX8: return "TGX8";
        case gm1::ArchiveType::TileObject: return "TileObject";
        case gm1::ArchiveType::Font: return "Font";
        case gm1::ArchiveType::Bitmap: return "Bitmap";
        default: return "Unknown";
        }
    }
    
    size_t GetPreambleSize(const gm1::Header &header)
    {
        size_t size = 0;

        /** About 88 bytes on gm1::Header **/
        size += gm1::CollectionHeaderBytes;

        /** About 10 palettes per file 512 bytes each **/
        size += gm1::CollectionPaletteCount * gm1::CollectionPaletteBytes;

        /** 32-bit size per entry **/
        size += header.imageCount * sizeof(uint32_t);

        /** 32-bit offset per entry **/
        size += header.imageCount * sizeof(uint32_t);
        
        /** Some gm1::EntryHeaders of 16 bytes long **/
        size += header.imageCount * gm1::CollectionEntryHeaderBytes;

        return size;
    }

    std::ostream& PrintEntryHeader(std::ostream &out, const EntryHeader &header)
    {
        using namespace std;
        out << "Width = "         << static_cast<int>(header.width) << endl
            << "Height = "        << static_cast<int>(header.height) << endl
            << "PosX = "          << static_cast<int>(header.posX) << endl
            << "PosY = "          << static_cast<int>(header.posY) << endl
            << "Group = "         << static_cast<int>(header.group) << endl
            << "GroupSize = "     << static_cast<int>(header.groupSize) << endl
            << "TileY = "         << static_cast<int>(header.tileY) << endl
            << "TileOrient = "    << static_cast<int>(header.tileOrient) << endl
            << "HorizOffset = "  << static_cast<int>(header.hOffset) << endl
            << "BoxWidth = "     << static_cast<int>(header.boxWidth) << endl
            << "Flags = "         << static_cast<int>(header.flags) << endl;
        return out;
    }

    std::ostream& PrintHeader(std::ostream &out, const Header &header)
    {
        using namespace std;
        out << "Unknown1 = "     << header.u1 << endl
            << "Unknown2 = "     << header.u2 << endl
            << "Unknown3 = "     << header.u3 << endl
            << "ImageCount= "   << header.imageCount << endl
            << "Unknown4 = "     << header.u4 << endl
            << "DataClass = "    << GetImageClassName(header.dataClass) << endl
            << "Unknown5 = "     << header.u5 << endl
            << "Unknown6 = "     << header.u6 << endl
            << "SizeCategory = " << header.sizeCategory << endl
            << "Unknown7 = "     << header.u7 << endl
            << "Unknown8 = "     << header.u8 << endl
            << "Unknown9 = "     << header.u9 << endl
            << "Width = "         << header.width << endl
            << "Height = "        << header.height << endl
            << "Unknown10 = "    << header.u10 << endl
            << "Unknown11 = "    << header.u11 << endl
            << "Unknown12 = "    << header.u12 << endl
            << "Unknown13 = "    << header.u13 << endl
            << "AnchorX = "      << header.anchorX << endl
            << "AnchorY = "      << header.anchorY << endl
            << "DataSize = "     << header.dataSize << endl
            << "Unknown14 = "    << header.u14 << endl;
        return out;
    }

    std::string GetSizeCategoryName(SizeCategory cat)
    {
        std::ostringstream oss;
        std::pair<int, int> dims = GetDimsBySizeCategory(cat);
        if((dims.first > 0) && (dims.second > 0)) {
            oss << dims.first << 'x' << dims.second;
            return oss.str();
        } else {
            return std::string("Undefined");
        }
    }
    
    SizeCategory ReadSizeCategory(std::istream &in)
    {
        uint32_t cat;
        core::ReadLittle(in, cat);
        return static_cast<SizeCategory>(cat);
    }
    
    std::ostream& WriteSizeCategory(std::ostream &out, SizeCategory cat)
    {
        core::WriteLittle(out, static_cast<uint32_t>(cat));
        return out;
    }
    
    SizeCategory GetSizeCategoryByDims(int width, int height)
    {
        for(unsigned n : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}) {
            SizeCategory cat = static_cast<SizeCategory>(n);
            auto dims = GetDimsBySizeCategory(cat);
            if((dims.first == width) && (dims.second == height))
                return cat;
        }
        
        return SizeCategory::Undefined;
    }
    
    const std::pair<int, int> GetDimsBySizeCategory(SizeCategory cat)
    {
        switch(cat) {
        case SizeCategory::Size30x30:
            return std::make_pair(30, 30);
        case SizeCategory::Size55x55:
            return std::make_pair(55, 55);
        case SizeCategory::Size75x75:
            return std::make_pair(75, 75);
        case SizeCategory::Size100x100:
            return std::make_pair(100, 100);
        case SizeCategory::Size110x110:
            return std::make_pair(110, 110);
        case SizeCategory::Size130x130:
            return std::make_pair(130, 130);
        case SizeCategory::Size180x180:
            return std::make_pair(180, 180);
        case SizeCategory::Size185x185:
            return std::make_pair(185, 185);
        case SizeCategory::Size250x250:
            return std::make_pair(250, 250);
        case SizeCategory::Unknown0:
        case SizeCategory::Unknown1:
        case SizeCategory::Undefined:
        default:
            return std::make_pair(0, 0);
        }
    }
    
} // namespace gm1
