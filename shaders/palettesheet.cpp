#include "palettesheet.h"

namespace Shaders
{
    GLuint LoadPaletteSheet(const std::vector<GM1::Palette> &palettes)
    {
        static const GLint NumMipmaps = 1;
        static const GLint XOffset = 0;
        static const GLint YOffset = 0;
        const GLint Height = palettes.size();
        static const GLint Width = GM1::CollectionPaletteColors;

        GLuint tex = 0;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_1D_ARRAY, tex);
        glTexStorage2D(GL_TEXTURE_1D_ARRAY, NumMipmaps, GL_RGBA, Width, Height);
        glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        std::vector<GM1::PaletteEntry> temp;
        for(auto &palette : palettes) {
            std::vector<GM1::PaletteEntry> entries(palette.begin(), palette.end());
            entries[0] = 0;
            for(auto &entry : entries)
                temp.push_back(entry);
        }

        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexSubImage2D(GL_TEXTURE_1D_ARRAY,
                        NumMipmaps-1,
                        XOffset,
                        YOffset,
                        Width,
                        Height,
                        GL_BGRA,
                        GL_UNSIGNED_SHORT_1_5_5_5_REV,
                        temp.data());
        
        return tex;
    }
}
