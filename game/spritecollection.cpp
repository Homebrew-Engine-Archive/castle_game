#include "spritecollection.h"

namespace castle
{
    namespace gfx
    {
        Sprite::Sprite(core::Image image, core::Point center)
            : mImage(std::move(image))
            , mCenter(std::move(center))
        {
        }
        
        const core::Image& Sprite::GetImage() const
        {
            return mImage;
        }
        
        const core::Point& Sprite::GetCenter() const
        {
            return mCenter;
        }
        
        SpriteCollection::SpriteCollection(const std::vector<BodyGroupDescription> &groups, const gm1::GM1Reader &reader)
            : mCollection(reader)
            , mLookupTable(groups)
        {
        }
        
        const Sprite SpriteCollection::GetSprite(const std::string &group, float frame, const core::Direction &dir)
        {
            const size_t index = mLookupTable.Get(group, frame, dir);

            const gm1::Header &header = mCollection.GetHeader();
            const core::Point center(header.anchorX, header.anchorY);
            
            return Sprite(mCollection.GetImage(index), center);
        }
    }
}
