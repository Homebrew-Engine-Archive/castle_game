#include "simulationmanager.h"

#include <memory>

#include <game/creaturestate.h>
#include <game/creature.h>
#include <game/simulationcontext.h>

namespace Castle {
    namespace World {
        SimulationManager::~SimulationManager() = default;
    
        SimulationManager::SimulationManager()
            : mPrimaryContext(new SimulationContext())
        {
        }
    
        void SimulationManager::Update(const std::chrono::milliseconds &elapsed)
        {
            const int turn = mPrimaryContext->GetTurn();
            mPrimaryContext->SetTurn(turn + 1);
        }
    
        bool SimulationManager::HasUpdate(const std::chrono::milliseconds &elapsed)
        {
            const int currentTurnNumber = mPrimaryContext->GetTurn();
            for(const PlayerAvatar &avatar : mAvatars) {
                const int turn = mAvatarTurn[avatar];
                if(turn <= currentTurnNumber) {
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
