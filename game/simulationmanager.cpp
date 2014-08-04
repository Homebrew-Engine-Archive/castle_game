#include "simulationmanager.h"

#include <memory>
#include <algorithm>

#include <game/creatureclass.h>
#include <game/creaturestate.h>
#include <game/creature.h>

namespace castle
{
    namespace world
    {
        SimulationManager::SimulationManager()
            : mTickDuration(std::chrono::milliseconds(10))
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
            const auto sinceLastTick = (elapsed + mTickInterpolation);
            const auto ticks = sinceLastTick / mTickDuration;
            mTickInterpolation = sinceLastTick % mTickDuration;
            AdvanceSimulation(ticks);
        }

        void SimulationManager::AdvanceSimulation(tick_t ticks)
        {
            for(Creature &creature : mCreatures) {
                creature.Update(ticks);
            }
        }
        
        void SimulationManager::RegisterClass(const std::string &name, CreatureClass cc)
        {
            mCreatureClasses.insert(std::make_pair(cc.GetName(), std::move(cc)));
        }
        
        void SimulationManager::SetMap(std::unique_ptr<Map> map)
        {
            mMap = std::move(map);
        }
            
        const Map& SimulationManager::GetMap() const
        {
            return *mMap;
        }
            
        void SimulationManager::AddCreature(const std::string &creatureClass, const CreatureState &st)
        {
            const CreatureClass &cc = mCreatureClasses.at(creatureClass);
            mCreatures.emplace_back(cc, st);
        }
    }
}
