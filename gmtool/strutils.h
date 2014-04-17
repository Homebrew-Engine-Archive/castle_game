#ifndef STRUTILS_H_
#define STRUTILS_H_

#include <string>
#include <sstream>

namespace StringUtils
{

    template<class ForwardIterator>
    std::string JoinStrings(ForwardIterator begin, ForwardIterator end, std::string sep, std::string lastSep = std::string(), std::string empty = std::string())
    {
        std::ostringstream oss;

        if(begin != end) {
            oss << *begin;
            ++begin;
        }

        while(begin != end) {
            typename ForwardIterator::value_type value = *begin;
            ++begin;
            if(lastSep.empty() || (begin != end)) {
                oss << sep << value;
            } else {
                oss << lastSep << value;
            }
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
