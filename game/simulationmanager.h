#ifndef SIMULATIONMANAGER_H_
#define SIMULATIONMANAGER_H_

#include <map>
#include <chrono>
#include <vector>
#include <memory>

#include <game/gamemap.h>

namespace Castle
{
    class SimulationCommand
    { };
    
    class SimulationManager
    {
        std::vector<int> mEntities;
        std::unique_ptr<Castle::GameMap> mMap;
        std::map<int, std::vector<SimulationCommand>> mBatchedCommands;

    public:
        SimulationManager();

        Castle::GameMap& GetGameMap();
        void SetGameMap(std::unique_ptr<Castle::GameMap> map);
        
        void Update();

        void InjectCommand(const SimulationCommand &command);
        
        bool HasUpdate(std::chrono::milliseconds elapsed);
    };
}

#endif // SIMULATIONMANAGER_H_
