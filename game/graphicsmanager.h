#ifndef GRAPHICSMANAGER_H_
#define GRAPHICSMANAGER_H_

#include <SDL.h>

#include <string>
#include <map>

#include <game/collection.h>
#include <game/landscape.h>
#include <game/filesystem.h>
#include <game/gm1palette.h>

class Surface;

namespace Render
{
    class Renderer;
}

namespace Graphics
{
    class GraphicsManager
    {
    public:
        GraphicsManager();
        virtual ~GraphicsManager();
    };
}

#endif // GRAPHICSMANAGER_H_
