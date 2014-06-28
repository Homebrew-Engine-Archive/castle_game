#include "imageclipper.h"

#include <exception>

namespace core
{
    ImageClipper::ImageClipper(core::Image &surface, const core::Rect &cliprect)
        : mImage(surface)
        , mOldClip()
    {
        if(!mImage.Null()) {
            mOldClip = surface.GetClipRect();
            surface.SetClipRect(cliprect);
        }
    }

    void ImageClipper::Rollback()
    {
        if(!mImage.Null()) {
            mImage.SetClipRect(mOldClip);
            mImage = nullptr;
        }
    }
    
    ImageClipper::~ImageClipper() throw()
    {
        try {
            if(!mImage.Null()) {
                Rollback();
            }
        } catch(const std::exception &error) {
            std::cerr << "unable to recover clip rect: " << error.what() << std::endl;
        }
    }
}
