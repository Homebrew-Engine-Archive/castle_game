#include "creatureclass.h"

namespace castle
{
    namespace world
    {
        CreatureClass::CreatureClass(const std::string &name, const castle::gfx::SpriteCollection &collection)
            : mName(name)
            , mSpriteCollection(collection)
        {
        }

        const std::string& CreatureClass::GetName() const
        {
            return mName;
        }
        
        const castle::gfx::SpriteCollection& CreatureClass::GetSpriteCollection() const
        {
            return mSpriteCollection;
        }

    }
}
