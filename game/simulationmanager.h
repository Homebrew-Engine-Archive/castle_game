#ifndef SIMULATIONMANAGER_H_
#define SIMULATIONMANAGER_H_

#include <chrono>
#include <unordered_set>

namespace Castle
{
    class SimulationManager
    {
    public:
        void Simulate();
        std::chrono::milliseconds UpdateInterval() const;
    };
}

#endif // SIMULATIONMANAGER_H_
