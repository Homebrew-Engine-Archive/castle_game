#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <map>
#include <string>
#include <vector>

#include <SDL2/SDL.h>

#include "gm1.h"
#include "surface.h"

enum class AnimationClass : Uint32 {
    Walk,
    Run,
    Idle1,
    Idle2,
    Idle3,
    Dig,
    Climb,
    RemoveLadder,
    DeathArrow,
    DeathMelee1,
    DeathMelee2,
    Fall,
    AttackMelee,
    Victory                                              // swordman, mace
};

enum class Direction : int {
    NorthEast,
    East,
    SouthEast,
    South,
    SouthWest,
    West,
    NorthWest,
    North
};

// Most of unit animations
static const std::vector<Direction> DIRECTION_FULL = {
    Direction::NorthEast,
    Direction::East,
    Direction::SouthEast,
    Direction::South,
    Direction::SouthWest,
    Direction::West,
    Direction::NorthWest,
    Direction::North
};

// I saw archers melee combat and maceman victory
static const std::vector<Direction> DIRECTION_MAIN = {
    Direction::East,
    Direction::South,
    Direction::West,
    Direction::North
};

// Most of deaths, idle, building animations
static const std::vector<Direction> DIRECTION_FRONT = {
    Direction::South
};

#endif
