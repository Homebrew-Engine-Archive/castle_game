#ifndef IMAGECLIPPER_H_
#define IMAGECLIPPER_H_

#include <core/rect.h>
#include <core/image.h>

namespace core
{
    class ImageClipper
    {
        Image mImage;
        core::Rect mOldClip;
    
    public:
        explicit ImageClipper(core::Image &surface, const core::Rect &cliprect);
        virtual ~ImageClipper() throw();

        void Rollback();
    };
}

#endif // IMAGECLIPPER_H_
