#include "creature.h"

#include <game/creaturestate.h>
#include <game/creatureclass.h>

namespace castle
{
    namespace world
    {
        Creature::~Creature() = default;
        Creature::Creature(const CreatureClass &cc, const CreatureState &state)
            : mClass(&cc)
            , mState(std::move(state))
        {
        }

        const CreatureClass& Creature::GetClass() const
        {
            return *mClass;
        }

        void Creature::SetClass(const CreatureClass &cc)
        {
            mClass = &cc;
        }

        const CreatureState& Creature::GetState() const
        {
            return mState;
        }

        CreatureState& Creature::GetState()
        {
            return mState;
        }
        
        void Creature::SetState(const CreatureState &state)
        {
            mState = state;
        }

        void Creature::Update(tick_t ticks)
        {
            mState.Update(ticks);
        }
    }
}
