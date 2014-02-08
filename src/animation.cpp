#include "animation.h"

FrameSequence::FrameSequence(int frameCount, const std::vector<Direction> &dirs,
                             const std::string &tag)
    : frameCount(frameCount)
    , dirs(dirs)
    , tag(tag)
{}
