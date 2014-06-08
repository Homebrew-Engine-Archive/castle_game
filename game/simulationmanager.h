#ifndef SIMULATIONMANAGER_H_
#define SIMULATIONMANAGER_H_

#include <map>
#include <chrono>
#include <vector>
#include <memory>

#include <game/simulationcommand.h>
#include <game/playeravatar.h>
#include <game/gamemap.h>

namespace Castle
{
    class SimulationManager
    {
        std::unique_ptr<Castle::GameMap> mMap;
        std::map<PlayerAvatar, std::vector<SimulationCommand>> mBatchedCommands;
        int mSimulationStep;
        PlayerAvatar mHostPlayer;
        PlayerAvatar mLocalPlayer;
        std::vector<PlayerAvatar> mPlayers;

    public:
        SimulationManager();
        static SimulationManager& Instance();

        Castle::GameMap& GetGameMap();
        void SetGameMap(std::unique_ptr<Castle::GameMap> map);
        
        void Update();

        PlayerAvatar GetLocalPlayerAvatar() const;
        PlayerAvatar GetHostPlayerAvatar() const;
        
        void InjectCommand(const PlayerAvatar &player, const SimulationCommand &command);
        
        bool HasUpdate(std::chrono::milliseconds elapsed);
    };
}

#endif // SIMULATIONMANAGER_H_
