#include "hexcolor.h"

#include <sstream>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

void validate(boost::any &v, const std::vector<std::string> &values, HexColor*, int)
{
    po::validators::check_first_occurrence(v);
    const std::string &s = po::validators::get_single_string(values);

    std::istringstream iss(s);

    /** #3fef98 is valid color such as 3f3f98 **/
    char sharp;
    if(iss >> sharp && sharp != '#') {
        iss.unget();
    }

    HexColor color;
    iss >> color;
    if(iss) {
        v = boost::any(color);
    } else {
        throw po::validation_error(po::validation_error::invalid_option_value);
    }
}

std::ostream& operator<<(std::ostream &out, const HexColor &color)
{
    auto flags = out.flags();
    out.width(8);
    out.fill('0');
    out << std::right << std::hex << color.value;
    out.flags(flags);
    return out;
}

std::istream& operator>>(std::istream &in, HexColor &color)
{
    auto flags = in.flags();
    in >> std::hex >> color.value;
    in.flags(flags);
    return in;
}
