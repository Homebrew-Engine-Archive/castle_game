#ifndef CREATURECLASS_H_
#define CREATURECLASS_H_

#include <string>

#include <game/gm1reader.h>
#include <game/vfs.h>
#include <game/spritecollection.h>
#include <game/spritelookuptable.h>

namespace castle
{
    namespace world
    {
        class CreatureClass
        {
        public:
            CreatureClass(const std::string &name, const std::vector<gfx::BodyGroupDescription> &groups, const gm1::GM1Reader &reader);
            
            /** represent creature name like "archer" or "seagull" **/
            const std::string& GetName() const;
            const castle::gfx::SpriteCollection& GetSpriteCollection() const;

        private:
            std::string mName;
            castle::gfx::SpriteCollection mSpriteCollection;
        }; 
    }
}

#endif // CREATURECLASS_H_
