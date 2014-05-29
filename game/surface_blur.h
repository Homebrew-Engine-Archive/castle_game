#ifndef SURFACE_BLUR_H_
#define SURFACE_BLUR_H

class Surface;

namespace Graphics
{
    void BlurSurface(Surface &surface, int radius);
    void BlurSurfaceAlpha(Surface &surface, int radius);
}

#endif // SURFACE_BLUR_H_
