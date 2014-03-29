#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <vector>
#include "direction.h"
#include "gm1.h"
#include "surface.h"
#include "filesystem.h"

enum class AnimationClass : int
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
