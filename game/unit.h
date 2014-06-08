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
        virtual bool CanClimb() const
            {return true;}
        virtual bool CanDig() const
            {return true;}
        virtual bool CanRun() const
            {return true;}
        virtual bool CanWalkWalls() const
            {return true;}
        virtual double Velocity() const
            {return 1.0f;}
        virtual bool HasRangeAttack() const
            {return true;}
        virtual bool HasMeleeAttack() const
            {return true;}
        virtual int Health() const
            {return 100;}
        virtual int RangeDamage() const
            {return 2;}
        virtual int MeleeDamage() const
            {return 1;}
        virtual bool Selectable() const
            {return true;}
        virtual int RangeAttackMaxDistance() const
            {return 1;}
        virtual int RangeAttackMinDistance() const
            {return 0;}
        virtual int MeleeAttackMaxDistance() const
            {return 1;}
        virtual int MeleeAttackMinDistance() const
            {return 1;}

        virtual UnitTrait* MeleeAttack() const
            {return new MeleeAttackTrait(1, 1, 2);}

        virtual UnitTrait* Movement() const
            {return new FastMovement();}
    };

    class SimulationContext;

    struct UnitTrait
    {
        virtual void Update(SimulationContext &context) = 0;
    };
    
    struct MeleeAttackTrait : public UnitTrait
    {
        MeleeAttackTrait(int minDistance, int maxDistance, int damage)
            {}
        virtual void Update(SimulationContext &context) {}
    };
    
    class ArcherTraits : public UnitTraits
    {
    public:
        
    };
    
    class Unit
    {
        const UnitTraits &mTraits;
        std::unique_ptr<UnitState> mState;
        
    public:
        Unit(const UnitTraits &traits, )
        
        void Update(double delta);
        
        void SetSprite(const SpriteSurface &sprite);
        Graphics::SpriteSurface const& GetSprite() const;
        UnitTraits const& GetTraits() const;
    };
}

#endif // UNIT_H_
