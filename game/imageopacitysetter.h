#ifndef IMAGEOPACITYSETTER_H_
#define IMAGEOPACITYSETTER_H_

#include <game/image.h>

namespace castle
{
    class ImageOpacitySetter
    {
        castle::Image mImage;
        uint8_t mOldAlphaMod;
        
    public:
        explicit ImageOpacitySetter(const castle::Image &src, uint8_t alpha);
        virtual ~ImageOpacitySetter() throw();

        void Rollback();
    };
}

#endif // IMAGEOPACITYSETTER_H_
