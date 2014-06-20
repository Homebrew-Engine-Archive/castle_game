#include "simulationmanager.h"

#include <memory>
#include <algorithm>

#include <game/creaturestate.h>
#include <game/creature.h>
#include <game/simulationcontext.h>

namespace castle
{
    namespace world
    {
        SimulationManager::~SimulationManager() = default;
    
        SimulationManager::SimulationManager()
            : mPrimaryContext(new SimulationContext())
            , mTurnLength(100)
            , mTurnNumber(0)
        {
        }

        void SimulationManager::CompleteTurn(const PlayerAvatar &player, int turn)
        {
            mAvatarTurn[player] = turn;
        }

        void SimulationManager::AddPlayer(const PlayerAvatar &player)
        {
            mAvatars.push_back(player);
        }
        
        void SimulationManager::RemovePlayer(const PlayerAvatar &player)
        {
            const auto end = std::remove(mAvatars.begin(), mAvatars.end(), player);
            mAvatars.erase(end, mAvatars.end());
        }
        
        void SimulationManager::Update(const std::chrono::milliseconds &elapsed)
        {
            for(const SimulationCommand &command : mCommands) {
                command.Execute(*mPrimaryContext);
            }
        }
    
        bool SimulationManager::HasUpdate(const std::chrono::milliseconds &elapsed)
        {
            for(const PlayerAvatar &avatar : mAvatars) {
                const int turn = mAvatarTurn[avatar];
                if(turn <= mTurnNumber) {
                    return false;
                }
            }
            return true;
        }

        SimulationContext& SimulationManager::PrimaryContext()
        {
            return *mPrimaryContext;
        }
    
        void SimulationManager::SetPrimaryContext(std::unique_ptr<SimulationContext> context)
        {
            mPrimaryContext = std::move(context);
        }
    }
}
