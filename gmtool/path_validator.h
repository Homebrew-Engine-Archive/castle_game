#ifndef PATH_VALIDATOR_H_
#define PATH_VALIDATOR_H_

#include <iostream>
#include <boost/filesystem/path.hpp>

namespace boost
{
    namespace filesystem
    {
        /// Special case overload for boost::filesystem::path. It is a little
        /// workaround of handling paths that contain spaces.
        inline std::istream& operator>>(std::istream &is, path &path)
        {
            path::string_type str;
            std::getline(is, str);
            path = str;
            return is;
        }
    }
}

#endif // PATH_VALIDATOR_H_
