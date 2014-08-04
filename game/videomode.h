#ifndef VIDEOMODE_H_
#define VIDEOMODE_H_

#include <cstdint>

namespace castle
{
    namespace render
    {
        class VideoMode
        {    
        public:
            VideoMode();                
            VideoMode(int width, int height, uint32_t format);
                
            bool operator==(const VideoMode &other) const;
            bool operator!=(const VideoMode &that) const;

            inline int Width() const;
            inline int Height() const;
            inline uint32_t Format() const;

            void SetWidth(int width);
            void SetHeight(int height);
            void SetFormat(int format);
            
        private:
            int mWidth;
            int mHeight;
            uint32_t mFormat;

        };

        inline int VideoMode::Width() const
        {
            return mWidth;
        }

        inline int VideoMode::Height() const
        {
            return mHeight;
        }

        inline uint32_t VideoMode::Format() const
        {
            return mFormat;
        }
    }
}

#endif // OUTPUTMODE_H_
