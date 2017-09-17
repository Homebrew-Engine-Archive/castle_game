#include "bodyanimation.h"

namespace castle
{
    namespace gfx
    {
        BodyAnimation::BodyAnimation(const std::string &name, castle::world::tick_t maxTicks, size_t frames)
            : mName(name)
            , mDuration(maxTicks)
            , mFrameCount(frames)
        {
        }
            
        const std::string& BodyAnimation::GetName() const
        {
            return mName;
        }
        
        castle::world::tick_t BodyAnimation::GetDuration() const
        {
            return mDuration;
        }
        
        size_t BodyAnimation::GetFrameCount() const
        {
            return mFrameCount;
        }
    }
}
