#include "creaturestate.h"

namespace Castle {
    namespace World {
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
