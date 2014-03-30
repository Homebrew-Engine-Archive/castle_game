#ifndef COMPONENT_H_
#define COMPONENT_H_

namespace Render
{
    class GraphicsManager;
}

namespace Castle
{

    class Component
    {
    public:
        virtual void OnGraphicsLoading(GraphicsManager&) { };
        virtual void OnFrameStarted(int64_t elapsed) { };
    };

}

#endif
