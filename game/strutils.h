#ifndef STRUTILS_H_
#define STRUTILS_H_

#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <boost/filesystem/fstream.hpp>
#include <game/filesystem.h>

namespace str
{
    template<class T>
    std::vector<T> ReadList(const fs::path &path)
    {
        std::vector<T> list;
    
        boost::filesystem::ifstream fin(path);
        T buffer;
        while(fin >> buffer) {
            if(!buffer.empty()) {
                list.push_back(std::move(buffer));
            }
        }

        return list;
    }


    template<class T>
    std::basic_string<T> LowerCase(const std::basic_string<T> &source)
    {
        std::basic_string<T> result = source;
        for(T &t : result) {
            t = tolower(t);
        }
        return result;
    }

    template<class T>
    std::basic_string<T> UpperCase(const std::basic_string<T> &source)
    {
        std::basic_string<T> result = source;
        for(T &t : result) {
            t = toupper(t);
        }
        return result;
    }

    template<class T, class U>
    std::basic_string<U> JoinList(const std::vector<T> &elements, const std::basic_string<U> &separator)
    {
        std::ostringstream oss;
        std::ostream_iterator<T> joinstream(oss, separator);
        std::copy(elements.begin(), elements.end(), joinstream);
        return oss.str();
    }
}

#endif // STRUTILS_H_
