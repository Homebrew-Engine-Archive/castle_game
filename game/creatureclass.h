#ifndef CREATURECLASS_H_
#define CREATURECLASS_H_

namespace castle
{
    namespace gfx
    {
        class Collection;
        class SpriteLookupTable;
    }
}

namespace castle
{
    namespace world
    {
        class CreatureClass
        {
        public:
            /** represent creature name like "archer" or "seagull" **/
            const std::string GetName() const;

            /** entire collection of images such as body_archer.gm1 **/
            const castle::gfx::Collection& GetSpriteCollection() const;

            /** how to get desired image from custom key (group, direction, frame) **/
            const castle::gfx::SpriteLookupTable& GetSpriteLookupTable() const;
        }; 
    }
}

#endif // CREATURECLASS_H_
