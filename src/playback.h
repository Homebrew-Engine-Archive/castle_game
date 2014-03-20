#ifndef PLAYBACK_H_
#define PLAYBACK_H_

#include "SDL.h"

class Animation
{
    std::vector<Surface> mFrames;
public:
    Animation(const std::vector<Surface>, size_t from, size_t stride, size_t count);
};

class Playback
{
public:
    Playback(const Surface &single);
    Playback(const Animation &anim, std::int64_t duration);
    void Advance(std::int64_t elapsed);
    Surface GetFrame() const;
    Playback Translate(const Animation &that);
};

#endif
