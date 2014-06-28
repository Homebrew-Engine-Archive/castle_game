#include "simulationcontext.h"

namespace castle
{
    namespace world
    {
        SimulationContext::SimulationContext(SimulationContext const&) = delete;
        SimulationContext& SimulationContext::operator=(SimulationContext const&) = delete;
        SimulationContext::~SimulationContext() = default;
    
        SimulationContext::SimulationContext()
        {
        }

        void SimulationContext::SetMap(std::unique_ptr<Map> map)
        {
            mMap = std::move(map);
        }

        unsigned SimulationContext::AddCreature(const Creature &creature)
        {
            const unsigned id = mCreatures.size();
            mCreatures.push_back(creature);
            return id;
        }
        
        void SimulationContext::RemoveCreature(unsigned id)
        {
            if(!mCreatures.empty()) {
                mCreatures.pop_back();
            }
        }
    }
}
