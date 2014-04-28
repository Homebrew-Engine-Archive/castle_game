#ifndef PALETTESHEET_H_
#define PALETTESHEET_H_

#include <vector>

#include <GL/glew.h>
#include <game/gm1palette.h>

namespace Shaders
{
    GLuint LoadPaletteSheet(const std::vector<GM1::Palette> &palettes);
}

#endif // PALETTESHEET_H_
