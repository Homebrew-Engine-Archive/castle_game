#ifndef CELLRENDERERPIPELINE_H_
#define CELLRENDERERPIPELINE_H_

namespace Castle
{
    class GameMap;
}

namespace Render
{
    class Renderer;
}

namespace Render
{
    class CellRenderMode
    {
    public:
        virtual Point WorldPosition(const Castle::GameMap::Cell &cell, const Point &point)
            {return point;}
        
    };
    
    class CellRendererPipeline
    {
    public:
        explicit CellRendererPipeline(Render::Renderer &renderer);
        
        void RenderCell(const Castle::GameMap::Cell &cell);

        void SetScaleMode(const CellRenderMode &mode);
        void SetRotationMode(const CellRenderMode &mode);
    };
}

#endif // CELLRENDERERPIPELINE_H_
