#ifndef CREATURE_H_
#define CREATURE_H_

#include <memory>

#include <game/playeravatar.h>

namespace Castle
{
    class CreatureState
    {
    public:
        virtual double GetX() const = 0;
        virtual double GetY() const = 0;
    };
    
    class Creature
    {
        PlayerAvatar mOwner;
        uint32_t mIdentity;
        std::unique_ptr<CreatureState> mState;
        
    public:
        Creature(int identity, std::unique_ptr<CreatureState> state)
            : mOwner()
            , mIdentity(identity)
            , mState(std::move(state))
            {}

        inline uint32_t Id() const;
        inline CreatureState const* GetState() const;
        inline CreatureState* GetState();
    };

    inline uint32_t Creature::Id() const
    {
        return mIdentity;
    }

    inline CreatureState const* Creature::GetState() const
    {
        return mState.get();
    }

    inline CreatureState* Creature::GetState()
    {
        return mState.get();
    }
    
    inline bool operator==(const Creature &alice, const Creature &bob)
    {
        return alice.Id() == bob.Id();
    }
}

#endif // CREATURE_H_
