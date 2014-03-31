#ifndef MAIN_H_
#define MAIN_H_

#include <iosfwd>
#include <SDL.h>

void EnumRenderDrivers(std::ostream &out);
void GetAndPrintRendererInfo(std::ostream &out, SDL_Renderer *renderer);

#endif
