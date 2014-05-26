#ifndef PATH_VALIDATOR_H_
#define PATH_VALIDATOR_H_

#include <iostream>
#include <boost/filesystem/path.hpp>

/**
 * \brief Overload for boost::filesystem::path
 * It is a little workaround of handling paths with spaces.
 */

inline std::istream& operator>>(std::istream &is, boost::filesystem::path &path)
{
    boost::filesystem::path::string_type str;
    std::getline(is, str);
    path = str;
    return is;
}

#endif // PATH_VALIDATOR_H_
