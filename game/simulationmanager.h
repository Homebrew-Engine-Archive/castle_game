#ifndef SIMULATIONMANAGER_H_
#define SIMULATIONMANAGER_H_

#include <map>
#include <chrono>
#include <vector>
#include <memory>

#include <game/creature.h>
#include <game/simulationcommand.h>
#include <game/simulationtick.h>
#include <game/playeravatar.h>
#include <game/gamemap.h>

namespace castle
{
    namespace gfx
    {
        class SpriteCollection;
    }
    namespace world
    {
        class CreatureClass;
    }
}

namespace castle
{
    namespace world
    {
        class SimulationManager
        {
        public:
            SimulationManager();

            void Update(const std::chrono::milliseconds &elapsed);
            
            void SetMap(std::unique_ptr<Map> map);
            const Map& GetMap() const;

            void RegisterClass(const std::string &name, CreatureClass cc);
            void AddCreature(const std::string &creatureClass, const CreatureState &st);
            
        private:
            void CompleteTurn(const PlayerAvatar &player, int turn);
            void AddPlayer(const PlayerAvatar &player);
            void RemovePlayer(const PlayerAvatar &player);

            void AdvanceSimulation(tick_t ticks);
            
        protected:
            std::unique_ptr<Map> mMap;
            std::vector<Creature> mCreatures;
            std::map<std::string, CreatureClass> mCreatureClasses;
            std::vector<PlayerAvatar> mAvatars;
            std::map<PlayerAvatar, unsigned> mAvatarTurn;
            std::vector<SimulationCommand> mCommands;
            std::chrono::nanoseconds mTickInterpolation;
            std::chrono::nanoseconds mTickDuration;
            unsigned mTurnLength;
            unsigned mTurnNumber;
            PlayerAvatar mHostAvatar;
            PlayerAvatar mLocalAvatar;
        };
    }
}

#endif // SIMULATIONMANAGER_H_
