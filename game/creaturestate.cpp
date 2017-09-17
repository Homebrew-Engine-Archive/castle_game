#include "creaturestate.h"

#include <cassert>

namespace castle
{
    namespace world
    {
        CreatureState::CreatureState(const std::string &anim, const core::Direction &dir, tick_t startedAt, castle::gfx::PaletteName palette)
            : mName(anim)
            , mDirection(dir)
            , mTicksElapsed(startedAt)
            , mPaletteName(palette)
        {
        }

        void CreatureState::SetName(const std::string &anim)
        {
            mName = anim;
        }

        void CreatureState::SetDirection(const core::Direction &dir)
        {
            mDirection = dir;
        }

        void CreatureState::ResetState()
        {
            mTicksElapsed = 0;
        }

        void CreatureState::SetPaletteName(const castle::gfx::PaletteName &palette)
        {
            mPaletteName = palette;
        }
        
        const std::string& CreatureState::GetName() const
        {
            return mName;
        }
        
        core::Direction CreatureState::GetDirection() const
        {
            return mDirection;
        }
        
        tick_t CreatureState::GetTicks() const
        {
            return mTicksElapsed;
        }
        
        castle::gfx::PaletteName CreatureState::GetPaletteName() const
        {
            return mPaletteName;
        }
        
        void CreatureState::Update(tick_t ticks)
        {
            mTicksElapsed += ticks;
        }
    }
}
