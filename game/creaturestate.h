#ifndef CREATURESTATE_H_
#define CREATURESTATE_H_

#include <map>
#include <string>

#include <core/direction.h>

namespace castle
{
    namespace world
    {
        class CreatureState
        {
        public:
            /** examples are "run", "walk" and "shoot" **/                
            const std::string GetName() const;
            core::Direction GetDirection() const;
            float GetCompleteRate() const;

            void Update(int ticks);
        };
    }
}

#endif // CREATURESTATE_H_
