#ifndef CREATURE_H_
#define CREATURE_H_

#include <memory>

#include <game/playeravatar.h>
#include <game/creaturestate.h>
#include <game/creatureclass.h>

namespace castle
{
    namespace world
    {
        class CreatureState;
        class CreatureClass;

        class Creature
        {
            const CreatureClass *mClass;
            CreatureState mState;

        public:
            explicit Creature(const CreatureClass &cc, const CreatureState &state);
            virtual ~Creature();

            const CreatureClass& GetClass() const;
            void SetClass(const CreatureClass &cc);
            
            CreatureState& GetState();
            const CreatureState& GetState() const;

            void SetState(const CreatureState &state);
        };
    }
}

#endif // CREATURE_H_
