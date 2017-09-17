#include "imageopacitysetter.h"

#include <exception>

namespace core
{
    ImageOpacitySetter::ImageOpacitySetter(const core::Image &src, uint8_t newAlphaMod)
        : mImage(src)
        , mOldAlphaMod(src.GetOpacity())
    {
        mImage.SetOpacity(newAlphaMod);
    }

    void ImageOpacitySetter::Rollback()
    {
        mImage.SetOpacity(mOldAlphaMod);
        mImage = nullptr;
    }

    ImageOpacitySetter::~ImageOpacitySetter() throw()
    {
        try {
            if(!mImage.Null()) {
                Rollback();
            }
        } catch(const std::exception &error) {
            std::cerr << "unable recover alpha modifier: " << error.what() << std::endl;
        }
    }
}
