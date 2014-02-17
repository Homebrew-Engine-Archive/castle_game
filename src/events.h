#ifndef EVENTS_H_
#define EVENTS_H_

#include <cstdint>
#include <SDL2/SDL.h>

enum class EventCode : uint32_t {
    DemandGM1Preload,
    CompleteGM1Preload
};

#endif
