#include "imagetransform.h"

#include <functional>

#include <SDL.h>

#include <core/imagelocker.h>
#include <core/image.h>
#include <core/color.h>

namespace
{
    class TransformFunctor
    {
        const SDL_PixelFormat &mFormat;
        std::function<core::Color(core::Color const&)> mFunc;

    public:
        inline TransformFunctor(const SDL_PixelFormat &format, core::Color func(core::Color const&));
        inline void operator()(char *bytes, size_t size);
    };

    inline TransformFunctor::TransformFunctor(const SDL_PixelFormat &format, core::Color func(core::Color const&))
        : mFormat(format)
        , mFunc(func)
    {}

    inline void TransformFunctor::operator()(char *bytes, size_t size)
    {
        const char *end = bytes + size * mFormat.BytesPerPixel;
        
        while(bytes != end) {
            const uint32_t origPixel = core::GetPackedPixel(bytes, mFormat.BytesPerPixel);

            core::Color color;
            SDL_GetRGBA(origPixel, &mFormat, &color.r, &color.g, &color.b, &color.a);

            const core::Color result = mFunc(color);
            const uint32_t pixel = SDL_MapRGBA(&mFormat, result.r, result.g, result.b, result.a);
            core::SetPackedPixel(bytes, pixel, mFormat.BytesPerPixel);

            bytes += mFormat.BytesPerPixel;
        }
    }
}

namespace core
{
    void TransformImage(Image &image, core::Color func(core::Color const&))
    {
        assert(!image.Null());
    
        ImageLocker lock(image);

        const auto width = image.Width();
        const auto rowStride = image.RowStride();
        char *const bytes = lock.Data();

        TransformFunctor transform(ImageFormat(image), func);

        for(size_t y = 0; y < image.Height(); ++y) {
            char *const data = bytes + rowStride * y;
            transform(data, width);
        }
    }
}
