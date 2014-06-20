#include "creaturestate.h"

namespace castle
{
    namespace world
    {
        DefaultCreatureState::DefaultCreatureState(double x, double y)
            : mX(x)
            , mY(y)
        {
        }
    
        double DefaultCreatureState::GetX() const
        {
            return mX;
        }
    
        double DefaultCreatureState::GetY() const
        {
            return mY;
        }

        const std::string DefaultCreatureState::GetSpriteName() const
        {
            return std::string();
        }
    
        int DefaultCreatureState::GetOpacity() const
        {
            return 255;
        }

        void DefaultCreatureState::Update(const SimulationContext &context, Creature &creature)
        {
        
        }
    }
}
