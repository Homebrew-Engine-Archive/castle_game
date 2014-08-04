#include "videomode.h"

namespace castle
{
    namespace render
    {
        VideoMode::VideoMode()
            : VideoMode(0, 0, 0)
        {
        }

        VideoMode::VideoMode(int width, int height, uint32_t format)
            : mWidth(width)
            , mHeight(height)
            , mFormat(format)
        {
        }

        bool VideoMode::operator==(const VideoMode &other) const
        {
            return other.mWidth == mHeight && other.mHeight == mHeight && other.mFormat == mFormat;
        }

        bool VideoMode::operator!=(const VideoMode &that) const
        {
            return !this->operator==(that);
        }

        void VideoMode::SetWidth(int width)
        {
            mWidth = width;
        }

        void VideoMode::SetHeight(int height)
        {
            mHeight = height;
        }

        void VideoMode::SetFormat(int format)
        {
            mFormat = format;
        }
    }
}
