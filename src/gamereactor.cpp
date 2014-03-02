#include "gamereactor.h"

Action::Action()
{
}

Action::Action(SDL_RWops *, Sint64 )
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
