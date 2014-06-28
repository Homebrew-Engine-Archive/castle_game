#ifndef BODYANIMATION_H_
#define BODYANIMATION_H_

#include <vector>
#include <cstddef>
#include <string>

#include <core/direction.h>
#include <game/simulationtick.h>

namespace castle
{
    namespace gfx
    {
        class BodyAnimation
        {
        public:
            BodyAnimation(const std::string &name, castle::world::tick_t maxTicks, size_t frames);
            
            const std::string& GetName() const;
            castle::world::tick_t GetDuration() const;
            size_t GetFrameCount() const;
            
        private:
            std::string mName;
            castle::world::tick_t mDuration;
            size_t mFrameCount;
        };
    }
}

#endif // BODYANIMATION_H_
