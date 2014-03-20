#ifndef GAMEREACTOR_H_
#define GAMEREACTOR_H_

#include "SDL.h"
#include "rw.h"

enum class ResourceCategory : Uint8 {
    Food, Material, Weapon, Stock, Other
};

enum class Resource : Uint8 {
    Bread, Apple, Cheese, Meat,
    Wood, Stone, Iron, Oil,
    Bow, Spear, Mace, LeatherArmor, Crossbow, Sword, MetalArmor,
    Wheat, Hop, Ale, Flour,
    Horse, Gold
};

enum class ActionClass : Uint8 {
    
};

enum class ActionResult : Uint8 {
    NotEnoughGold,
    NotEnoughWood,
    NotEnoughStone,
    NotEnoughIron,
    NotEnoughOil    
};

class Action
{
public:
    Action();
    Action(SDL_RWops *src, Sint64 numBytes);

    bool Null() const;
};

class GameReactor
{
    bool mNetworking;
    bool mServer;
public:
    GameReactor();
    ~GameReactor();
};

void PushAction(const Action &i, GameReactor &reactor);

#endif
