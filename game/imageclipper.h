#ifndef IMAGECLIPPER_H_
#define IMAGECLIPPER_H_

#include <game/rect.h>
#include <game/image.h>

namespace castle
{
    class ImageClipper
    {
        Image mImage;
        core::Rect mOldClip;
    
    public:
        explicit ImageClipper(castle::Image &surface, const core::Rect &cliprect);
        virtual ~ImageClipper() throw();

        void Rollback();
    };
}

#endif // IMAGECLIPPER_H_
