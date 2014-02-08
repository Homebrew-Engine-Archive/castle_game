#ifndef MAIN_H_
#define MAIN_H_

#include <iostream>
#include <fstream>
#include <initializer_list>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

#include <SDL2/SDL.h>

#include "animation.h"
#include "SDLContext.h"
#include "errors.h"
#include "tgx.h"
#include "gm1.h"

void DrawIsometricAxes(SDL_Renderer *renderer, int cx, int cy, int width, int height);

void DrawFrontalAxes(SDL_Renderer *renderer, int cx, int cy, int width, int height);

template<class OutputIterator>
void LoadStringList(const std::string &filename, OutputIterator output);

struct Animation
{
    Animation(const GM1Header &header, const std::vector<Frame> &frames);
    size_t ShiftIndex(int n) const;
    size_t GetNextIndex() const;
    size_t GetPrevIndex() const;
    void Next();
    void Prev();    
    const Frame& CurrentFrame() const;
    int CurrentOffset() const;
    int EvalOffset(const Frame &frame) const;
    const std::vector<Frame> &frames;
    std::vector<int> offsets;
    size_t index;
    const GM1Header &header;
    SDL_Rect Box() const;
    SDL_Rect ShiftedBox(int dx, int dy) const;
};

#endif
