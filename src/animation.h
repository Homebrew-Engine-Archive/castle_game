#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <vector>
#include "direction.h"

enum class AnimationClass : uint32_t {
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

// Archer::Archer(const gm1::Collection &collection, const std::vector<Frame> &frames)
// {
//     AnimationBuilder builder(collection, frames);
//     builder.Read(16, DIRECTION_FULL, walk);
//     builder.Read(16, DIRECTION_FULL, run);
//     builder.Read(24, DIRECTION_FULL, shot);
//     builder.Read(12, DIRECTION_FULL, shotDown);
//     builder.Read(12, DIRECTION_FULL, shotUp);
//     builder.Read(12, DIRECTION_MAIN, tilt);
//     builder.Read(16, aware);
//     builder.Read(16, idle);
//     builder.Read(24, deathArrow);
//     builder.Read(24, deathMelee1);
//     builder.Read(24, deathMelee2);
//     builder.Read(12, DIRECTION_FULL, attack);
//     builder.Read(12, DIRECTION_FULL, climb);
//     builder.Read(8, DIRECTION_FULL, fall);
//     builder.Read(16, DIRECTION_FULL, dig);
// }

#endif
