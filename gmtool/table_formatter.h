#ifndef TABLE_FORMATTER_H_
#define TABLE_FORMATTER_H_

#include <iosfwd>
#include <string>

namespace gmtool
{
    void PrintColumn(std::ostream &out, const std::string &text, int minWidth = 0, int indent = 3);
}

#endif // TABLE_FORMATTER_H_
