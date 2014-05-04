#include "simulationmanager.h"

namespace Castle
{
    void SimulationManager::Simulate()
    {

    }

    std::chrono::milliseconds SimulationManager::UpdateInterval() const
    {
        return std::chrono::milliseconds(10);
    }
}
