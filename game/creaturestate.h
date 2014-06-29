#ifndef CREATURESTATE_H_
#define CREATURESTATE_H_

#include <map>
#include <string>

#include <game/bodyanimation.h>
#include <game/simulationtick.h>
#include <core/direction.h>

namespace castle
{
    namespace world
    {
        class Map;
    }
    
    namespace gfx
    {
        enum class PaletteName;
    }
}

namespace castle
{
    namespace world
    {
        class CreatureState
        {
        public:
            CreatureState(const std::string &name, const core::Direction &dir, tick_t startedAt, castle::gfx::PaletteName palette);
            
            const std::string& GetName() const;
            core::Direction GetDirection() const;
            tick_t GetTicks() const;
            castle::gfx::PaletteName GetPaletteName() const;
            int GetOpacity() const;
            int GetWorldX() const;
            int GetWorldY() const;

            void Update(tick_t ticks);
            void ResetState();
            void SetName(const std::string &anim);
            void SetDirection(const core::Direction &dir);
            void SetPaletteName(const castle::gfx::PaletteName &name);

        private:
            std::string mName;
            core::Direction mDirection;
            castle::world::tick_t mTicksElapsed;
            castle::gfx::PaletteName mPaletteName;
        };
    }
}

#endif // CREATURESTATE_H_
