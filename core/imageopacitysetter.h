#ifndef IMAGEOPACITYSETTER_H_
#define IMAGEOPACITYSETTER_H_

#include <core/image.h>

namespace core
{
    class ImageOpacitySetter
    {
        core::Image mImage;
        uint8_t mOldAlphaMod;
        
    public:
        explicit ImageOpacitySetter(const core::Image &src, uint8_t alpha);
        virtual ~ImageOpacitySetter() throw();

        void Rollback();
    };
}

#endif // IMAGEOPACITYSETTER_H_
