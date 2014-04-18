#ifndef HEXCOLOR_H_
#define HEXCOLOR_H_

#include <iosfwd>
#include <boost/program_options.hpp>

struct HexColor
{
    uint32_t value;
};
    
void validate(boost::any &v, const std::vector<std::string> &values, HexColor *target_type, int);

std::ostream& operator<<(std::ostream&, HexColor const&);
std::istream& operator>>(std::istream&, HexColor&);

#endif
