#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <vector>
#include <game/direction.h>
#include <game/gm1.h>
#include <game/surface.h>
#include <game/filesystem.h>

enum class AnimType : int
{
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

#endif
