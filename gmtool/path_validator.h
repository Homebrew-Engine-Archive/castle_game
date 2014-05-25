#ifndef PATH_VALIDATOR_H_
#define PATH_VALIDATOR_H_

#include <iostream>
#include <boost/filesystem/path.hpp>

inline std::istream& operator>>(std::istream &is, boost::filesystem::path &path)
{
    boost::filesystem::path::string_type str;
    std::getline(is, str);
    path = str;
    return is;
}

#endif // PATH_VALIDATOR_H_
