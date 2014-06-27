#include "imageblur.h"

#include <cassert>

#include <vector>
#include <stdexcept>

#include <SDL.h>

#include <game/imagelocker.h>
#include <game/image.h>

#include <core/color.h>

namespace
{
    class ConvolveFunctor
    {
        std::vector<uint32_t> mBuffer;
        uint32_t *const mRedBuff;
        uint32_t *const mGreenBuff;
        uint32_t *const mBlueBuff;
        const int mRadius;
        const SDL_PixelFormat &mFormat;

    public:
        inline ConvolveFunctor(int radius, const SDL_PixelFormat &format, int bufferSize);
        inline void operator()(char *bytes, int length, int stride);
    };

    inline ConvolveFunctor::ConvolveFunctor(int radius, const SDL_PixelFormat &format, int bufferSize)
        : mBuffer(bufferSize * 3)
        , mRedBuff(mBuffer.data())
        , mGreenBuff(mBuffer.data() + bufferSize)
        , mBlueBuff(mBuffer.data() + bufferSize * 2)
        , mRadius(radius)
        , mFormat(format)
    {}

    inline void ConvolveFunctor::operator()(char *bytes, int length, int stride)
    {    
        uint32_t redAccum = 0;
        uint32_t greenAccum = 0;
        uint32_t blueAccum = 0;

        for(int i = 0; i < length; ++i) {
            const uint32_t pixel = core::GetPackedPixel(bytes + i * stride, mFormat.BytesPerPixel);

            uint8_t r;
            uint8_t g;
            uint8_t b;
            SDL_GetRGB(pixel, &mFormat, &r, &g, &b);

            redAccum += r;
            greenAccum += g;
            blueAccum += b;

            mRedBuff[i] = redAccum;
            mGreenBuff[i] = greenAccum;
            mBlueBuff[i] = blueAccum;
        }

        for(int i = 0; i < length; ++i) {
            const size_t minIndex = std::max(0, i - mRadius);
            const size_t maxIndex = std::min(i + mRadius, length - 1);

            const uint32_t red = mRedBuff[maxIndex] - mRedBuff[minIndex];
            const uint32_t green = mGreenBuff[maxIndex] - mGreenBuff[minIndex];
            const uint32_t blue = mBlueBuff[maxIndex] - mBlueBuff[minIndex];
            const uint32_t count = maxIndex - minIndex;

            const uint32_t pixel = SDL_MapRGB(&mFormat, red / count, green / count, blue / count);
            core::SetPackedPixel(bytes + i * stride, pixel, mFormat.BytesPerPixel);
        }
    }
}

namespace castle
{
    namespace gfx
    {
        void BlurImage(Image &dst, size_t radius)
        {
            if(!dst) {
                throw std::invalid_argument("surface is null or invalid");
            }

            const auto buffSize = std::max(dst.Width(),
                                           dst.Height());

            ConvolveFunctor convolve(radius, ImageFormat(dst), buffSize);
    
            if(radius < 1 || radius > buffSize) {
                throw std::invalid_argument("inproper convolution radius");
            }

            ImageLocker lock(dst);
            char *const dataBegin = lock.Data();
    
            /** Per row convolution **/
            for(size_t y = 0; y < dst.Height(); ++y) {
                char *const data = dataBegin + dst.RowStride() * y;
                const auto count = dst.Width();
                const auto stride = dst.PixelStride();
                convolve(data, count, stride);
            }

            /** Per column convolution **/
            for(size_t x = 0; x < dst.Width(); ++x) {
                char *const data = dataBegin + dst.PixelStride() * x;
                const auto count = dst.Height();
                const auto stride = dst.RowStride();
                convolve(data, count, stride);
            }
        }

        void BlurImageAlpha(Image &surface, size_t radius)
        {

        }
    }
}
