#include "creature.h"

#include <game/simulationcontext.h>
#include <game/creaturestate.h>
#include <game/creatureclass.h>

namespace castle
{
    namespace world
    {
        Creature::~Creature() = default;
        Creature::Creature(const CreatureClass &cc, std::unique_ptr<CreatureState> state)
            : mClass(cc)
            , mState(std::move(state))
        {
        }
    }
}
