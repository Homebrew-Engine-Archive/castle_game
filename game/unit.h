#ifndef UNIT_H_
#define UNIT_H_

#include <memory>

#include <game/spritepack.h>

namespace Castle
{
    class Unit;
    
    class UnitState
    {
    protected:
        Unit &mSelf;
        
    public:
        void Update(double delta);
        Unit& Self();
    };
    
    class UnitTraits
    {
    public:
        
    };

    class Unit
    {
        const UnitTraits &mTraits;
        std::unique_ptr<UnitState> mState;
        
    public:
        void Update(double delta);
        
        void SetSprite(const SpriteSurface &sprite);
        Graphics::SpriteSurface const& GetSprite() const;
        UnitTraits const& GetTraits() const;
    };
}

#endif // UNIT_H_
