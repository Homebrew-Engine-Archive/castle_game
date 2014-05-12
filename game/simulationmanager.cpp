#include "simulationmanager.h"

namespace Castle
{
    SimulationManager::SimulationManager()
        : mEntities()
        , mMap(nullptr)
        , mBatchedCommands()
    {

    }

    Castle::GameMap& SimulationManager::GetGameMap()
    {
        return *mMap;
    }
    
    void SimulationManager::Update()
    {

    }

    bool SimulationManager::HasUpdate(std::chrono::milliseconds elapsed)
    {
        return elapsed > std::chrono::milliseconds(100);
    }
}
