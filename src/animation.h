#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <vector>
#include <SDL2/SDL.h>

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

int GetFramesCount(AnimationClass anim)
{
    switch(anim) {
    case Walk: return 16;                                // mace
    case Run: return 16;                                 // mace
    case Idle1: return 16;                               // mace
    case Idle2: return 16;                               // mace
    case Idle3: return 24;                               // swordman
    case DeathArrow: return 24;                          // mace, swordman
    case DeathMelee: return 24;                          // swordman
    case Dig: return 16;                                 // mace
    case Fall: return 8;                                 // mace
    case RemoveLadder: return 1;                         // Pikemen detected
    case AttackMelee: return 24;                         // swordman
    case Victory: return 16;                             // swordman
    }
}

#endif
