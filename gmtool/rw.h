#ifndef RW_H_
#define RW_H_

#include <iosfwd>
#include <SDL.h>

SDL_RWops* SDL_RWFromOutputStream(std::ostream&);
SDL_RWops* SDL_RWFromInputStream(std::istream&);

#endif // RW_H_
