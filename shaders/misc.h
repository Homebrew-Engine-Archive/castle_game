#ifndef MISC_H_
#define MISC_H_

#include <string>
#include <vector>
#include <iostream>

namespace Misc
{
    template<class Char>
    std::vector<Char> LoadString(const std::string &filename)
    {
        std::ifstream fin(filename, std::ios_base::in);
        if(!fin) {
            throw std::runtime_error(strerror(errno));
        }

        fin.seekg(0, std::ios_base::end);
        std::streampos fsize = fin.tellg();
        fin.seekg(0);
        
        std::vector<Char> buffer(fsize, 0);
        fin.read(&buffer[0], fsize);
        
        return buffer;
    }
}

#endif // MISC_H_
