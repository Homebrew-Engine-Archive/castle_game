#include "hexcolor.h"

#include <sstream>
#include <string>

#include <game/color.h>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

void validate(boost::any &v, const std::vector<std::string> &values, Color*, int)
{
    po::validators::check_first_occurrence(v);
    const std::string &s = po::validators::get_single_string(values);

    std::istringstream iss(s);
    Color color;
    iss >> color;
    
    if(iss) {
        v = boost::any(color);
    } else {
        throw po::validation_error(po::validation_error::invalid_option_value);
    }
}
