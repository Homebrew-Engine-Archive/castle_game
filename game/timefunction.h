#ifndef TIMEFUNCTION_H_
#define TIMEFUNCTION_H_

#include <chrono>

class TimeFunction
{
    double mInit;
    double mDest;
    std::chrono::milliseconds mLatency;
    std::chrono::milliseconds mElapsed;
public:
    TimeFunction(double value)
        : mInit(value)
        , mDest(value)
        , mLatency(1)
        , mElapsed(1)
        {}

    void Set(std::chrono::milliseconds delay, double from, double to) {
        mInit = from;
        mDest = to;
        mLatency = delay;
        mElapsed = std::chrono::milliseconds::zero();
    }
    
    double Get() const {
        if(mLatency.count() != 0) {
            return mInit + (mDest - mInit) * (mElapsed.count() / mLatency.count());
        } else {
            return mDest;
        }
    }

    void Update(std::chrono::milliseconds delta) {
        if(mElapsed != mLatency) {
            mElapsed = std::min(mElapsed + delta, mLatency);
        }
    }

    TimeFunction& operator+=(double value) {
        mDest += value;
        return *this;
    }

    TimeFunction& operator=(double value) {
        mDest = value;
        return *this;
    }
};

#endif // TIMEFUNCTION_H_
