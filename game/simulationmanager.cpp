#include "simulationmanager.h"

namespace Castle
{
    SimulationManager::SimulationManager()
        : mMap(nullptr)
        , mBatchedCommands()
        , mSimulationStep(0)
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
        mSimulationStep += 1;
    }

    PlayerAvatar SimulationManager::GetLocalPlayerAvatar() const
    {
        return mLocalPlayer;
    }

    PlayerAvatar SimulationManager::GetHostPlayerAvatar() const
    {
        return mHostPlayer;
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
