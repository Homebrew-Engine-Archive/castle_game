#include "simulationmanager.h"

namespace Castle
{
    void SimulationManager::Update()
    {

    }

    bool SimulationManager::HasUpdate(std::chrono::milliseconds elapsed)
    {
        return elapsed > std::chrono::milliseconds(100);
    }
}
