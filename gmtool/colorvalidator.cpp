#include "colorvalidator.h"

#include <sstream>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

ColorValue::ColorValue(uint32_t value)
    : mValue(value)
{ }

uint32_t ColorValue::Get() const
{
    return mValue;
}

uint32_t ColorValue::Set(uint32_t value)
{
    uint32_t oldValue = mValue;
    mValue = value;
    return oldValue;
}

void validate(boost::any &v, const std::vector<std::string> &values, ColorValue*, int)
{
    po::validators::check_first_occurrence(v);
    const std::string &s = po::validators::get_single_string(values);

    std::istringstream iss(s);

    /** #3fef98 is valid color too **/
    char sharp;
    if(iss >> sharp && sharp != '#') {
        std::cout << sharp << std::endl;
        iss.unget();
    }
    
    uint32_t rawColor = 0;
    iss >> std::hex >> rawColor;
    if(iss) {
        v = boost::any(ColorValue(rawColor));
    } else {
        throw po::validation_error(po::validation_error::invalid_option_value);
    }
}

std::ostream& operator<<(std::ostream &out, const ColorValue &color)
{
    auto flags = out.flags();
    out.setf(flags | std::ios::hex);
    out << color.Get();
    out.flags(flags);
    return out;
}

std::istream& operator>>(std::istream &in, ColorValue &color)
{
    uint32_t value = 0;
    auto flags = in.flags();
    in.setf(flags | std::ios::hex);
    in >> value;
    color.Set(value);
    in.flags(flags);
    return in;
}
