#ifndef GAMEREACTOR_H_
#define GAMEREACTOR_H_

#include <SDL.h>
#include "rw.h"

enum class ResourceCategory : uint8_t {
    Food, Material, Weapon, Stock, Other
};

enum class Resource : uint8_t {
    Bread, Apple, Cheese, Meat,
    Wood, Stone, Iron, Oil,
    Bow, Spear, Mace, LeatherArmor, Crossbow, Sword, MetalArmor,
    Wheat, Hop, Ale, Flour,
    Horse, Gold
};

enum class ActionClass : uint8_t {
    
};

enum class ActionResult : uint8_t {
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
    Action(SDL_RWops *src, int64_t numBytes);

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
