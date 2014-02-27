#include <cstdio>
#include <string>
#include <stdexcept>
#include <SDL2/SDL.h>
#include "gm1.h"
#include "tgx.h"

using namespace std;

const char * GetTokenName(int tt)
{
    switch(tt) {
    case 0: return "Stream";
    case 1: return "Transparent";
    case 2: return "Repeat";
    case 4: return "Newline";
    default: return "Unknown";
    }
}

int DecompileTGX(SDL_RWops *src, int size, int width, int height, int bytesPerPixel)
{
    int write = 0;
    int read = 0;
    while(read < size) {
        Uint8 token;
        if(1 != SDL_RWread(src, &token, sizeof(Uint8), 1))
            throw std::runtime_error("Overrun");
        int tt = token >> 5;
        int tl = (token & 0x1f) + 1;
        int tb = tl * bytesPerPixel;
        read += sizeof(Uint8);
        printf("at %d\t%02X\ttt=%s\ttl=%d\n", read, token, GetTokenName(tt), tl);
        continue;
        switch(tt) {
        case 0:
            {
                printf("at %d\t%02X\ttt=Stream\ttl=%d\n", read, token, tl);
                if(tb != 0) {
                    printf("\t");
                    for(int i = 0; i < tb; ++i) {
                        Uint8 p;
                        if(1 != SDL_RWread(src, &p, sizeof(Uint8), 1))
                            throw std::runtime_error("Overrun");
                        printf(" %02X", p);
                    }
                    printf("\n");
                }
                if(read + tb  > size)
                    printf("Size exceed by %d\n", (read + tb  - size));
                read += tb;
                write += tl;
            }
            break;

        case 1:
            {
                printf("at %d\t%02X\ttt=Transparent\tLength=%d\n", read, token, tl);
                write += tl;
            }
            break;

        case 2:
            {
                printf("at %d\t%02X\tRepeat\tLength=%d\n", read, token, tl);
                if(read + bytesPerPixel > size)
                    printf("Size exceed by %d\n", (read + bytesPerPixel - size));
                char buffer[4];
                if(1 != SDL_RWread(src, buffer, bytesPerPixel, 1))
                    throw std::runtime_error("Overrun");
                printf("\t");
                for(int i = 0; i < bytesPerPixel; ++i) {
                    printf(" %02X", buffer[i]);
                }
                printf("\n");
                read += bytesPerPixel;
                write += tl;
            }
            break;

        case 4:
            {
                size_t skip = width - (write % width);
                printf("at %d\t%02X\ttt=Newline\tLength=%d Skip=%d\n", read, token, tl, skip);
                write += skip;
            }
            break;

        default:
            {
                printf("at %d\t%02X\ttt=%d\ttl=%d\n", read, token, tt, tl);
            }
        }
    }
    return 0;
}

const char * GetEncodingName(gm1::Encoding enc)
{
    switch(enc) {
    case gm1::Encoding::TGX16: return "TGX16";
    case gm1::Encoding::TGX8: return "TGX8";
    case gm1::Encoding::TileObject: return "TileObject";
    case gm1::Encoding::Bitmap: return "Bitmap";
    default:
        return "Unknown";
    }
}

int tgx_main(const std::string &filename, int index)
{
    SDL_RWops *src = SDL_RWFromFile(filename.c_str(), "rb");
    try {
        if(src == NULL)
            throw std::runtime_error("Can't open file");
        gm1::Collection gm1(src);
        int offset = gm1.offsets.at(index);
        int size = gm1.sizes.at(index);
        gm1::ImageHeader hdr = gm1.headers.at(index);
        SDL_RWseek(src, offset, RW_SEEK_CUR);

        auto enc = gm1::GetEncoding(gm1.header);
        SDL_Log("Encoding: %s", GetEncodingName(enc));
        SDL_Log("Width: %d, Height: %d", hdr.width, hdr.height);

        switch(enc) {
        case gm1::Encoding::TGX16:
            {
                return DecompileTGX(src, size, hdr.width, hdr.height, 2);
            }
            break;

        case gm1::Encoding::TGX8:
            {
                return DecompileTGX(src, size, hdr.width, hdr.height, 1);
            }
            break;
        default:
            throw std::runtime_error("Only tgx16 or tgx8 acceptable");
            
        }
    } catch(const std::exception &e) {
        SDL_Log("Exception: %s", e.what());
        return -1;
    }
    if(src != 0)
        SDL_RWclose(src);
    return 0;
}

int main(int argc, char *argv[])
{
    if(argc != 3) {
        SDL_Log("Usage: tgx <gm1filename> <index>");
        return -1;
    }

    std::string filename = argv[1];
    int index = stoi(std::string(argv[2]));

    SDL_Log("Filename: %s, Index: %d", filename.c_str(), index);
                
    return tgx_main(filename, index);
}
