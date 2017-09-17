#ifndef UNIT_H_
#define UNIT_H_

#include <map>
#include <game/simulationtick.h>

namespace castle
{
    namespace world
    {
        enum class Trigger {
            Wet, Fire, Disease, Heal
        };
        
        class UnitState
        {
        public:
            virtual void Update(tick_t ticks, class Unit &unit) = 0;
        };

        struct Unit {
            float health;
            float diseaseResistance;
            float fireResistance;
        };

        
        class TriggeredState : public UnitState
        {
            std::map<Trigger, bool> mState;
        public:
            void SetTrigger(const Trigger &trigger, bool state) {
                mState[trigger] = state;
            }
            
            virtual void Update(tick_t ticks, Unit &unit) {
                if(mState[Trigger::Disease]) {
                    unit.health -= unit.diseaseResistance * ticks;
                }
                if(mState[Trigger::Fire] && !mState[Trigger::Wet]) {
                    unit.health -= unit.fireResistance * ticks;
                }
                if(mState[Trigger::Heal]) {
                    unit.health += ticks;
                }
            }
        };
    }
}

#endif // UNIT_H_
