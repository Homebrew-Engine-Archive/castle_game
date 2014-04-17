#ifndef COLORVALIDATOR_H_
#define COLORVALIDATOR_H_

#include <iosfwd>

#include <boost/program_options.hpp>

class ColorValue
{
    uint32_t mValue;
public:
    ColorValue(uint32_t value = 0);
    uint32_t Get() const;
    uint32_t Set(uint32_t value);
};
    
void validate(boost::any &v, const std::vector<std::string> &values, ColorValue *target_type, int);

std::ostream& operator<<(std::ostream&, ColorValue const&);
std::istream& operator>>(std::istream&, ColorValue&);

#endif
