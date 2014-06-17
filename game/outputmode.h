#ifndef OUTPUTMODE_H_
#define OUTPUTMODE_H_

namespace Castle
{
    namespace Render
    {
        class OutputMode
        {
            int mWidth;
            int mHeight;
            uint32_t mFormat;
    
        public:
            OutputMode()
                : OutputMode(0, 0, 0)
                {}
    
            OutputMode(int width, int height, uint32_t format)
                : mWidth(width)
                , mHeight(height)
                , mFormat(format)
                {}

            bool operator==(const OutputMode &other) const {
                return other.mWidth == mHeight && other.mHeight == mHeight && other.mFormat == mFormat;
            }

            bool operator!=(const OutputMode &that) const {
                return !this->operator==(that);
            }

            auto Width() const -> decltype(mWidth) {
                return mWidth;
            }

            auto Height() const -> decltype(mHeight) {
                return mHeight;
            }

            auto Format() const -> decltype(mFormat) {
                return mFormat;
            }

            void SetWidth(int width) {
                mWidth = width;
            }

            void SetHeight(int height) {
                mHeight = height;
            }

            void SetFormat(int format) {
                mFormat = format;
            }
        };
    }
}

#endif // OUTPUTMODE_H_
