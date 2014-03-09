#ifndef ATLAS_H_
#define ATLAS_H_

#include <algorithm>
#include <vector>
#include "surface.h"
#include "SDL.h"

/**
 * This one eval layout of surfaces and call BuildAtlas.
 *
 * @param surfaces      Their we would blit together.
 * @param partition     It would be filled up with rectangles surface.
 * @param maxWidth      Constraint on width of texture.
 * @param maxHeight     Constraint on height of texture
 *
 * @return              Resulted surface which contains all surfaces.
 *
 */
Surface MakeSurfaceAtlas(const std::vector<Surface> &surfaces, std::vector<SDL_Rect> &partition, int maxWidth, int maxHeight);

/**
 * And this one is just blit one surface after another with given partition.
 *
 * @param surfaces      Given set of surfaces.
 * @param partition     Previously evaluated layout.
 *
 * @return              Newly created surfaces with all surfaces blit together.
 *
 */
Surface BuildAtlas(const std::vector<Surface> &surfaces, const std::vector<SDL_Rect> &partition);

#endif
