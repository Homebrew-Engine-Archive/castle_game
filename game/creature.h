#ifndef CREATURE_H_
#define CREATURE_H_

#include <memory>

#include <game/creaturestate.h>
#include <game/playeravatar.h>
 
namespace Castle {
    namespace World {
        class Creature
        {
            PlayerAvatar mOwner;
            uint32_t mIdentity;
            std::unique_ptr<CreatureState> mState;
        
        public:
            explicit Creature(int identity, std::unique_ptr<CreatureState> state);

            inline uint32_t Id() const;
            inline CreatureState const& GetState() const;
            inline CreatureState& GetState();
        
            void SetState(std::unique_ptr<CreatureState> state);

            void Update(const SimulationContext &context);
        };

        inline uint32_t Creature::Id() const
        {
            return mIdentity;
        }

        inline CreatureState const& Creature::GetState() const
        {
            return *mState;
        }

        inline CreatureState& Creature::GetState()
        {
            return *mState;
        }
    
        inline bool operator==(const Creature &alice, const Creature &bob)
        {
            return alice.Id() == bob.Id();
        }
    }
}

#endif // CREATURE_H_
