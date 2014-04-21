#include "hexcolor.h"

#include <sstream>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

void validate(boost::any &v, const std::vector<std::string> &values, SDL_Color*, int)
{
    po::validators::check_first_occurrence(v);
    const std::string &s = po::validators::get_single_string(values);

    std::istringstream iss(s);

    /** #3fef98 is valid color such as 3f3f98 **/
    char sharp;
    if(iss >> sharp && sharp != '#') {
        iss.unget();
    }

    SDL_Color color;
    iss >> color;
    if(iss) {
        v = boost::any(color);
    } else {
        throw po::validation_error(po::validation_error::invalid_option_value);
    }
}
