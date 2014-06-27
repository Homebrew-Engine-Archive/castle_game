#ifndef RW_H_
#define RW_H_

#include <iosfwd>
#include <SDL.h>

namespace core
{
/**
   Constructing SDL_RWops from std::i/ostream!
   It made possible to use boost::filesystem::wpath with SDL.
**/

// \todo specialise for std::basic_iostream

    SDL_RWops* SDL_RWFromOutputStream(std::basic_ostream<char>&);
    SDL_RWops* SDL_RWFromInputStream(std::basic_istream<char>&);
}

#endif // RW_H_
