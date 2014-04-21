#ifndef HEXCOLOR_H_
#define HEXCOLOR_H_

#include <iosfwd>
#include <boost/program_options.hpp>

#include <game/sdl_utils.h>

#include <SDL.h>

void validate(boost::any &v, const std::vector<std::string> &values, SDL_Color *target_type, int);

#endif
