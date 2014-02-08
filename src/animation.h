#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <string>
#include <vector>
#include <SDL2/SDL.h>

#include "gm1.h"

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

// int GetFramesCount(AnimationClass anim)
// {
//     switch(anim) {
//     case AnimationClass::Walk: return 16;                // maceman
//     case AnimationClass::Run: return 16;                 // maceman
//     case AnimationClass::Idle1: return 16;               // maceman
//     case AnimationClass::Idle2: return 16;               // maceman
//     case AnimationClass::Idle3: return 24;               // swordman
//     case AnimationClass::DeathArrow: return 24;          // maceman, showrdman
//     case AnimationClass::DeathMelee1: return 24;         // swordman
//     case AnimationClass::Dig: return 16;                 // maceman
//     case AnimationClass::Fall: return 8;                 // maceman
//     case AnimationClass::RemoveLadder: return 1;         // pikeman
//     case AnimationClass::AttackMelee: return 24;         // swordman
//     case AnimationClass::Victory: return 16;             // swordman
//     }
// }

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
static const std::vector<Direction> EIGHT_DIRECTION = {
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
static const std::vector<Direction> FOUR_DIRECTION = {
    Direction::East,
    Direction::South,
    Direction::West,
    Direction::North
};

// Most of deaths, idle, building animations
static const std::vector<Direction> FRONT_ONLY = {
    Direction::South
};

struct FrameSequence
{
    int frameCount;
    const std::vector<Direction> &dirs;
    const std::string &tag;
    FrameSequence(int frameCount, const std::vector<Direction> &dirs, const std::string &tag);
};

#endif
