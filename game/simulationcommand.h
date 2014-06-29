#ifndef SIMULATIONCOMMAND_H_
#define SIMULATIONCOMMAND_H_

#include <game/playeravatar.h>

namespace castle
{
    namespace world
    {
        class SimulationCommand
        {
            PlayerAvatar mPlayer;
        public:
            explicit SimulationCommand(PlayerAvatar v)
                : mPlayer(v)
                {}
        };
    }
}

#endif // SIMULATIONCOMMAND_H_
