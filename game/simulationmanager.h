#ifndef SIMULATIONMANAGER_H_
#define SIMULATIONMANAGER_H_

#include <map>
#include <chrono>
#include <vector>
#include <memory>

#include <game/simulationcontext.h>
#include <game/creature.h>
#include <game/simulationcommand.h>
#include <game/playeravatar.h>
#include <game/gamemap.h>

namespace Castle
{
    class SimulationManager
    {
    private:
        SimulationManager(SimulationManager const&) = delete;
        SimulationManager& operator=(SimulationManager const&) = delete;
        
    public:
        explicit SimulationManager();
        virtual ~SimulationManager();

        void Update(const std::chrono::milliseconds &elapsed);
        bool HasUpdate(const std::chrono::milliseconds &elapsed);
        void SetPrimaryContext(std::unique_ptr<SimulationContext> context);
        SimulationContext& PrimaryContext();

    protected:
        std::vector<PlayerAvatar> mAvatars;
        std::map<PlayerAvatar, int> mAvatarTurn;
        std::unique_ptr<SimulationContext> mPrimaryContext;
    };
}

#endif // SIMULATIONMANAGER_H_
