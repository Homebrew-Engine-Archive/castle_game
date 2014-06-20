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
    }
}
