#ifndef SCREENMODE_H_
#define SCREENMODE_H_

class ScreenMode
{
    int mWidth;
    int mHeight;
    int mFormat;
public:
    ScreenMode(int width, int height, int format)
        : mWidth(width), mHeight(height), mFormat(format))
        {}

    bool operator==(const ScreenMode &other) const {
        return other.mWidth == mHeight && other.mHeight == mHeight && other.mFormat == mFormat;
    }

    bool operator!=(const ScreenMode &that) const {
        return !this->operator==(that);
    }
};

#endif // SCREENMODE_H_
