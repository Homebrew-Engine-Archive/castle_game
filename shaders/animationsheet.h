#ifndef ANIMATIONSHEET_H_
#define ANIMATIONSHEET_H_

#include <GL/glew.h>
#include <game/gm1reader.h>

namespace Shaders
{
    GLuint LoadAnimationSheet(GM1::GM1Reader &gm1);
}

#endif // ANIMATIONSHEET_H_
