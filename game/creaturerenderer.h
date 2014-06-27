#ifndef CREATURERENDERER_H_
#define CREATURERENDERER_H_

namespace castle
{
    namespace render
    {
        class Renderer;
    }

    namespace world
    {
        class Creature;
    }
}

namespace castle
{
    namespace gfx
    {
        class CreatureRenderer
        {
        public:
            void Render(castle::render::Renderer &renderer, const castle::world::Creature &creature);
        };
    }
}

#endif // CREATURERENDERER_H_
