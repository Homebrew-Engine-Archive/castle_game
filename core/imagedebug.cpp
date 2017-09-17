#include "imagedebug.h"

#include <iostream>
#include <core/image.h>

namespace core
{
    ImageDebug::ImageDebug(const Image &image)
        : mImg(image)
    {
    }
    
    ImageDebug::~ImageDebug() = default;

    std::ostream& ImageDebug::FormatName(std::ostream &out) const
    {
        out << "Format: "
            << SDL_GetPixelFormatName(core::ImageFormat(mImg).format)
            << std::endl;
        return out;
    }
    
    std::ostream& ImageDebug::BlendModeName(std::ostream &out) const
    {
        out << "BlendMode: ";
        switch(mImg.GetBlendMode()) {
        case SDL_BLENDMODE_BLEND:
            out << "blend";
            break;
        case SDL_BLENDMODE_NONE:
            out << "none";
            break;
        case SDL_BLENDMODE_ADD:
            out << "add";
            break;
        case SDL_BLENDMODE_MOD:
            out << "mod";
            break;
        default:
            out << "uncognized";
            break;
        }
        out << std::endl;
        return out;
    }
    
    std::ostream& ImageDebug::AlphaMod(std::ostream &out) const
    {
        out << "AlphaMod: " << mImg.GetOpacity() << std::endl;
        return out;
    }
    
    std::ostream& ImageDebug::Dimensions(std::ostream &out) const
    {
        out << "Dimensions: " << mImg.Width() << ' ' << mImg.Height() << std::endl;
        return out;
    }
    std::ostream& ImageDebug::ColorTable(std::ostream &out) const
    {
        const SDL_Palette *palette = core::ImageFormat(mImg).palette;
        out << "Palette: ";
        if(palette == nullptr) {
            out << "null";
        } else {
            out << palette->ncolors;
        }
        out << std::endl;
        return out;
    }

    std::ostream& ImageDebug::RefCount(std::ostream &out) const
    {
        out << "RefCount: " << mImg.GetSurface()->refcount << std::endl;
        return out;
    }
}
