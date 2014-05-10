#ifndef SIMULATIONMANAGER_H_
#define SIMULATIONMANAGER_H_

#include <chrono>
#include <vector>

namespace Castle
{
    class SimulationCommand
    { };
    
    class SimulationManager
    {
        std::vector<SimulationCommand> mBatchedCommands;
    public:
        void Update();

        void InjectCommand(const SimulationCommand &command);
        
        bool HasUpdate(std::chrono::milliseconds elapsed);
    };
}

#endif // SIMULATIONMANAGER_H_
