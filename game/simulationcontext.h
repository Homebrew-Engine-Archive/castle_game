#ifndef SIMULATIONCONTEXT_H_
#define SIMULATIONCONTEXT_H_

#include <cassert>

#include <chrono>
#include <memory>
#include <vector>

#include <game/gamemap.h>
#include <game/playeravatar.h>
#include <game/simulationcommand.h>
#include <game/creature.h>

namespace castle
{
    namespace world
    {
        class SimulationContext
        {
            std::unique_ptr<Map> mMap;
            std::vector<std::unique_ptr<Creature>> mCreatures;
        
        public:
            explicit SimulationContext();
            SimulationContext(SimulationContext const&);
            SimulationContext& operator=(SimulationContext const&);
            virtual ~SimulationContext();

            void SetMap(std::unique_ptr<Map> map);
            inline Map& GetMap();
        };

        inline Map& SimulationContext::GetMap()
        {
            return *mMap;
        }
    }
}

#endif // SIMULATIONCONTEXT_H_
