#ifndef HEXCOLOR_H_
#define HEXCOLOR_H_

#include <boost/any.hpp>

#include <vector>
#include <string>

class Color;

void validate(boost::any &v, const std::vector<std::string> &values, Color*, int);

#endif
