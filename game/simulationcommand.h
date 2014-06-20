#ifndef SIMULATIONCOMMAND_H_
#define SIMULATIONCOMMAND_H_

#include <game/playeravatar.h>

namespace castle
{
    namespace world
    {
        class SimulationContext;
    }
    
    namespace world
    {
        class SimulationCommand
        {
            PlayerAvatar mPlayer;
        public:
            explicit SimulationCommand(PlayerAvatar v)
                : mPlayer(v)
                {}

            void Execute(SimulationContext &context) const;
        };
    }
}

#endif // SIMULATIONCOMMAND_H_
