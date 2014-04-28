#include "animationsheet.h"

#include "glerror.h"

#include <game/gm1entryreader.h>
#include <game/sdl_utils.h>
#include <game/surface.h>

namespace Shaders
{
    GLuint LoadAnimationSheet(GM1::GM1Reader &gm1)
    {
        static const GLint NumMipmaps = 1;
        static const GLint XOffset = 0;
        static const GLint YOffset = 0;
        const GLsizei width = gm1.Header().width;
        const GLsizei height = gm1.Header().height;
        const GLsizei depth = gm1.NumEntries();
        
        GLuint tex = 0;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
        // Can't do it this way (available since gl4)
        // glTexStorage3D(GL_TEXTURE_2D_ARRAY, NumMipmaps, GL_R8, width, height, depth);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, NumMipmaps-1, GL_R8, width, height, depth, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        GM1::GM1EntryReader &reader = gm1.EntryReader();
        for(GLsizei i = 0; i < depth; ++i) {
            const Surface surface = reader.Load(gm1, i);
            const SurfaceLocker locker(surface);
            
            glPixelStorei(GL_UNPACK_ROW_LENGTH, surface->pitch);
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                            NumMipmaps-1,
                            XOffset,
                            YOffset,
                            i,
                            width,
                            height,
                            1,
                            GL_RED,
                            GL_UNSIGNED_BYTE,
                            surface->pixels);
        }
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

        return tex;
    }
}
