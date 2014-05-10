#ifndef HEXCOLOR_H_
#define HEXCOLOR_H_

#include <boost/any.hpp>
#include <game/color.h>

void validate(boost::any &v, const std::vector<std::string> &values, Color*, int);

#endif
