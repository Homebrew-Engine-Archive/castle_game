#include "main.h"

#include "../game/gm1reader.h"
#include "../game/sdl_utils.h"
#include "../game/surface.h"
#include "../game/gm1.h"

#include <fstream>

int main(int argc, char *argv[])
{
    if(argc < 3) {
        std::cout << "Use string and then integer, luke!" << std::endl;
        return 0;
    }

    std::string file = argv[1];
    size_t index = std::stoi(argv[2]);
    
    GM::GM1Reader reader(file);
    std::ofstream fout("data.bin", std::ofstream::binary);

    if(reader.NumEntries() <= index)
        throw std::runtime_error("No such index in the file");
    
    const char *data = reinterpret_cast<const char*>(reader.EntryData(index));

    GM::PrintHeader(std::cout, reader.Header());
    GM::PrintImageHeader(std::cout, reader.ImageHeader(index));
    
    fout.write(data, reader.EntrySize(index));
    
    return 0;
}
