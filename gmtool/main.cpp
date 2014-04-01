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
    int index = std::stoi(argv[2]);
    
    GM1::GM1Reader reader(file);
    std::ofstream fout("data.out", std::ios_base::binary);

    if(reader.NumEntries() <= index)
        throw std::runtime_error("No such index in the file");
    
    const char *data = reinterpret_cast<const char*>(reader.EntryData(index));

    GM1::PrintHeader(std::cout, reader.Header());
    GM1::PrintEntryHeader(std::cout, reader.EntryHeader(index));
    
    fout.write(data, reader.EntrySize(index));
    
    return 0;
}
