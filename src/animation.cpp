#include "animation.h"

AnimationBuilder::AnimationBuilder(const GM1CollectionScheme &scheme, const std::vector<Frame> &frames)
    : scheme(scheme)
    , frames(frames)
    , position(0)
{
}

void AnimationBuilder::Read(size_t frameCount, const std::vector<Direction> &dirs, std::map<Direction, std::vector<Frame>> &result)
{
    for(size_t index = 0; index < frameCount; ++index) {
        for(const auto &dir : dirs) {
            result[dir].push_back(frames[position]);
            ++position;
        }
    }
}

void AnimationBuilder::Read(size_t frameCount, std::vector<Frame> &result)
{
    for(size_t index = 0; index < frameCount; ++index) {
        result.push_back(frames[position]);
        ++position;
    }
}

Archer::Archer(const GM1CollectionScheme &scheme, const std::vector<Frame> &frames)
{
    AnimationBuilder builder(scheme, frames);
    builder.Read(16, DIRECTION_FULL, walk);
    builder.Read(16, DIRECTION_FULL, run);
    builder.Read(24, DIRECTION_FULL, shot);
    builder.Read(12, DIRECTION_FULL, shotDown);
    builder.Read(12, DIRECTION_FULL, shotUp);
    builder.Read(12, DIRECTION_MAIN, tilt);
    builder.Read(16, aware);
    builder.Read(16, idle);
    builder.Read(24, deathArrow);
    builder.Read(24, deathMelee1);
    builder.Read(24, deathMelee2);
    builder.Read(12, DIRECTION_FULL, attack);
    builder.Read(12, DIRECTION_FULL, climb);
    builder.Read(8, DIRECTION_FULL, fall);
    builder.Read(16, DIRECTION_FULL, dig);
}
