#ifndef ENTITYCLASS_H_
#define ENTITYCLASS_H_

namespace Render
{
    class GraphicsManager;
}

namespace Entities
{

    void LoadGraphics(Render::GraphicsManager&);
    
    class EntityClass
    {
    public:
        virtual void LoadGraphics(Render::GraphicsManager&) = 0;
    };

}

#endif
