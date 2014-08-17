#include "table_formatter.h"

#include <iostream>

namespace gmtool
{
    void PrintColumn(std::ostream &out, const std::string &text, int minWidth, int indent)
    {
        out.width(indent);
        out << out.fill();
        out.width(minWidth);
        out << std::left << text;
    }
}
