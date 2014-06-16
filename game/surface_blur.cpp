#include "surface_blur.h"

#include <cassert>

#include <vector>

#include <SDL.h>

#include <game/color.h>
#include <game/surface.h>
#include <stdexcept>

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
        ConvolveFunctor(int radius, const SDL_PixelFormat &format, int bufferSize)
            : mBuffer(bufferSize * 3)
            , mRedBuff(mBuffer.data())
            , mGreenBuff(mBuffer.data() + bufferSize)
            , mBlueBuff(mBuffer.data() + bufferSize * 2)
            , mRadius(radius)
            , mFormat(format)
            { }
        
        void operator()(char *bytes, int length, int stride)
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
    };
}

namespace Graphics
{
    void BlurSurface(Surface &dst, int radius)
    {
        if(!dst) {
            throw std::invalid_argument("surface is null or invalid");
        }

        const auto buffSize = std::max(SurfaceWidth(dst),
                                       SurfaceHeight(dst));

        ConvolveFunctor convolve(radius, SurfaceFormat(dst), buffSize);
    
        if(radius < 1 || radius > buffSize) {
            throw std::invalid_argument("inproper convolution radius");
        }

        const SurfaceLocker lock(dst);
        char *const dataBegin = SurfaceData(dst);
    
        /** Per row convolution **/
        for(int y = 0; y < SurfaceHeight(dst); ++y) {
            char *const data = dataBegin + SurfaceRowStride(dst) * y;
            const auto count = SurfaceWidth(dst);
            const auto stride = SurfacePixelStride(dst);
            convolve(data, count, stride);
        }

        /** Per column convolution **/
        for(int x = 0; x < SurfaceWidth(dst); ++x) {
            char *const data = dataBegin + SurfacePixelStride(dst) * x;
            const auto count = SurfaceHeight(dst);
            const auto stride = SurfaceRowStride(dst);
            convolve(data, count, stride);
        }
    }

    void BlurSurfaceAlpha(Surface &surface, int radius)
    {

    }
}
