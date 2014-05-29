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

    void SimulationManager::SetGameMap(std::unique_ptr<Castle::GameMap> map)
    {
        mMap = std::move(map);
    }
    
    void SimulationManager::Update()
    {

    }

    bool SimulationManager::HasUpdate(std::chrono::milliseconds elapsed)
    {
        return elapsed > std::chrono::milliseconds(100);
    }

    SimulationManager& SimulationManager::Instance()
    {
        static SimulationManager simManager;
        return simManager;
    }
}
