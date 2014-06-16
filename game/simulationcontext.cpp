#include "simulationcontext.h"

namespace Castle
{
    SimulationContext::SimulationContext(SimulationContext const&) = delete;
    SimulationContext& SimulationContext::operator=(SimulationContext const&) = delete;
    SimulationContext::~SimulationContext() = default;
    
    SimulationContext::SimulationContext()
        : mSimTurn(0)
        , mTurnLength(15)
        , mTurnLengthMultiplier(1.0f)
    {
    }

    void SimulationContext::SetTurn(int turn)
    {
        mSimTurn = turn;
    }
    
    void SimulationContext::SetGameMap(std::unique_ptr<GameMap> map)
    {
        mMap = std::move(map);
    }

    void SimulationContext::SetTurnLength(std::chrono::milliseconds length)
    {
        mTurnLength = length;
    }
    
    void SimulationContext::SetTurnMultiplier(float multiplier)
    {
        mTurnLengthMultiplier = multiplier;
    }
}
