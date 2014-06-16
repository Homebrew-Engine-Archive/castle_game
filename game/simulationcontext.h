#ifndef SIMULATIONCONTEXT_H_
#define SIMULATIONCONTEXT_H_

#include <cassert>

#include <chrono>
#include <memory>
#include <vector>

#include <game/gamemap.h>
#include <game/playeravatar.h>
#include <game/simulationcommand.h>
#include <game/creature.h>

namespace Castle
{
    namespace World
    {
        class SimulationContext
        {
            std::unique_ptr<Map> mMap;
            int mSimTurn;
            std::chrono::milliseconds mTurnLength;
            float mTurnLengthMultiplier;
            std::vector<std::unique_ptr<Creature>> mCreatures;
            
        
        public:
            explicit SimulationContext();
            SimulationContext(SimulationContext const&);
            SimulationContext& operator=(SimulationContext const&);
            virtual ~SimulationContext();

            void SetTurn(int turn);
            void SetMap(std::unique_ptr<Map> map);
            void SetTurnLength(std::chrono::milliseconds length);
            void SetTurnMultiplier(float multiplier);

            inline Map const& GetMap() const;

            inline int GetTurn() const;
            inline const std::chrono::milliseconds GetTurnLength() const;
            inline float GetTurnLengthMultiplier() const;
        };
    
        inline Map const& SimulationContext::GetMap() const
        {
            assert(mMap != nullptr);
            return *mMap;
        }

        inline int SimulationContext::GetTurn() const
        {
            return mSimTurn;
        }

        inline const std::chrono::milliseconds SimulationContext::GetTurnLength() const
        {
            return mTurnLength;
        }

        inline float SimulationContext::GetTurnLengthMultiplier() const
        {
            return mTurnLengthMultiplier;
        }
    }
}

#endif // SIMULATIONCONTEXT_H_
