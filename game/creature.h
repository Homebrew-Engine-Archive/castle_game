#ifndef CREATURE_H_
#define CREATURE_H_

#include <memory>

#include <game/playeravatar.h>
 
namespace castle
{
    namespace world
    {
        class CreatureState;
        class CreatureClass;

        class Creature
        {
            const CreatureClass &mClass;
            std::unique_ptr<CreatureState> mState;

        public:
            explicit Creature(const CreatureClass &cc, std::unique_ptr<CreatureState> state);
            virtual ~Creature();

            const CreatureClass& GetClass() const;
            const CreatureState& GetState() const;
            CreatureState& GetState();
        };
    }
}

#endif // CREATURE_H_
