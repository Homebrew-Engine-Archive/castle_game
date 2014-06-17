#ifndef CREATURESTATE_H_
#define CREATURESTATE_H_

#include <map>
#include <string>

#include <game/direction.h>

namespace Castle
{
    namespace World
    {
        class Creature;
        class SimulationContext;
    }
    
    namespace World
    {
        class CreatureState
        {
        public:
            virtual double GetX() const = 0;
            virtual double GetY() const = 0;
            virtual double GetZ() const = 0;

            virtual const std::string GetSpriteName() const = 0;
            virtual int GetOpacity() const = 0;
        
            virtual void Update(const SimulationContext &context, Creature &creature) = 0;
        };

        struct Archer
        {
            float srcX;
            float srcY;
            float srcZ;
            float dstX;
            float dstY;
            float dstZ;
            core::Direction dir;

            enum class ArcherState : int {
                Idle, Idle2, Walk, Run, Shot, Melee, DigUp, DigDown, ClimbUp, ClimbDown, Fall, Spawning, Disbanding, DeathMelee, DeathMelee2, DeathArrow, Propel
            };

            ArcherState state;
        };
        
        class DefaultCreatureState : public CreatureState
        {
            double mX;
            double mY;
            
        public:
            explicit DefaultCreatureState(double x, double y);

            virtual double GetX() const;
            virtual double GetY() const;

            virtual const std::string GetSpriteName() const;
            virtual int GetOpacity() const;

            virtual void Update(const SimulationContext &context, Creature &creature);
        };
    }
}

#endif // CREATURESTATE_H_
