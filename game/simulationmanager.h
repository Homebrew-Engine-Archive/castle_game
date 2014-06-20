#ifndef SIMULATIONMANAGER_H_
#define SIMULATIONMANAGER_H_

#include <map>
#include <chrono>
#include <vector>
#include <memory>

#include <game/creature.h>
#include <game/simulationcommand.h>
#include <game/simulationcontext.h>
#include <game/playeravatar.h>
#include <game/gamemap.h>

namespace castle
{
    namespace world
    {
        class SimulationManager
        {
        public:
            explicit SimulationManager();
            SimulationManager(SimulationManager const&) = delete;
            SimulationManager& operator=(SimulationManager const&) = delete;
            virtual ~SimulationManager();

            void Update(const std::chrono::milliseconds &elapsed);
            bool HasUpdate(const std::chrono::milliseconds &elapsed);
            void SetPrimaryContext(std::unique_ptr<SimulationContext> context);
            SimulationContext& PrimaryContext();

        private:
            void CompleteTurn(const PlayerAvatar &player, int turn);
            void AddPlayer(const PlayerAvatar &player);
            void RemovePlayer(const PlayerAvatar &player);
            
        protected:
            std::vector<PlayerAvatar> mAvatars;
            std::map<PlayerAvatar, int> mAvatarTurn;
            std::vector<SimulationCommand> mCommands;
            std::unique_ptr<SimulationContext> mPrimaryContext;
            std::chrono::milliseconds mTurnLength;
            std::chrono::milliseconds mSecondLength;
            int mTurnNumber;
            PlayerAvatar mThisPlayer;
            PlayerAvatar mHostPlayer;
        };
    }
}

#endif // SIMULATIONMANAGER_H_
