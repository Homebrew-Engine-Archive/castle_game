#include "gamereactor.h"

Action::Action()
{
}

Action::Action(SDL_RWops *src, Sint64 numBytes)
{
    
}

bool Action::Null() const
{
    return true;
}

GameReactor::GameReactor()
{
}

GameReactor::~GameReactor()
{

}

void PushAction(const Action &i, GameReactor &reactor)
{
    
}
