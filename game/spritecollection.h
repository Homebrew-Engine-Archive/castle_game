#ifndef SPRITECOLLECTION_H_
#define SPRITECOLLECTION_H_

#include <set>

#include <game/collection.h>
#include <game/spritelookuptable.h>
#include <game/gm1reader.h>

#include <core/point.h>
#include <core/image.h>

namespace castle
{
    namespace gfx
    {
        class BodyGroupDescription;
    }
    
    namespace gfx
    {
        class Sprite
        {
        public:
            Sprite(core::Image image, core::Point center);
            const core::Image& GetImage() const;
            const core::Point& GetCenter() const;
        private:
            core::Image mImage;
            core::Point mCenter;
        };

        class SpriteCollection
        {
        public:
            SpriteCollection(const std::vector<BodyGroupDescription> &groups, const gm1::GM1Reader &reader);
            
            const Sprite GetSprite(const std::string &group, float frame, const core::Direction &dir);
            
        private:
            castle::gfx::Collection mCollection;
            castle::gfx::SpriteLookupTable mLookupTable;
        };
    }
}

#endif // SPRITECOLLECTION_H_
