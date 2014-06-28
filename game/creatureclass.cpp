#include "creatureclass.h"

namespace castle
{
    namespace world
    {
        CreatureClass::CreatureClass(const std::string &name, const std::vector<gfx::BodyGroupDescription> &groups, const gm1::GM1Reader &reader)
            : mName(name)
            , mSpriteCollection(groups, reader)
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
