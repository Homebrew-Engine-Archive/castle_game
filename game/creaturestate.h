#ifndef CREATURESTATE_H_
#define CREATURESTATE_H_

#include <map>
#include <string>

namespace Castle {
    namespace World {
        class Creature;
        class SimulationContext;
    }
    namespace World {
        class CreatureState
        {
        public:
            virtual double GetX() const = 0;
            virtual double GetY() const = 0;

            virtual const std::string GetSpriteName() const = 0;
            virtual int GetOpacity() const = 0;
        
            virtual void Update(const SimulationContext &context, Creature &creature) = 0;
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
