#include "landscape.h"

Tile::Tile(LandscapeClass land, int height)
    : height(height)
    , landscape(land)
{
}

void Tile::Draw(Renderer &) const
{
    switch(landscape) {    
    case LandscapeClass::Pebbles:
        break;
    case LandscapeClass::Stones:
        break;
    case LandscapeClass::Rocks:
        break;
    case LandscapeClass::Boulders:
        break;
    case LandscapeClass::Dirt:
        break;
    case LandscapeClass::Grass:
        break;
    case LandscapeClass::Land:
        break;
    case LandscapeClass::Iron:
        break;
    case LandscapeClass::Beach:
        break;
    case LandscapeClass::River:
        break;
    case LandscapeClass::Ford:
        break;
    case LandscapeClass::Ripple:
        break;
    case LandscapeClass::Swamp:
        break;
    case LandscapeClass::Oil:
        break;
    case LandscapeClass::Sea:
        break;
    case LandscapeClass::Mash:
        break;
    }
}
