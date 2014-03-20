#include "gamereactor.h"

Action::Action()
{
}

Action::Action(SDL_RWops *, int64_t )
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

void PushAction(const Action &, GameReactor &)
{
    
}
