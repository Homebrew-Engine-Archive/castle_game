#ifndef STRUTILS_H_
#define STRUTILS_H_

#include <string>
#include <sstream>

namespace StringUtils
{

    template<class ForwardIterator>
    std::string JoinStrings(ForwardIterator begin, ForwardIterator end, std::string sep, std::string empty = std::string())
    {
        std::ostringstream oss;

        if(begin != end) {
            oss << *begin;
            ++begin;
        }

        while(begin != end) {
            oss << sep << *begin;
            ++begin;
        }

        std::string temp = oss.str();
        if(temp.empty()) {
            return empty;
        } else {
            return temp;
        }
    }

}

#endif
