#include <iostream>
#include <game/engine.h>

int main()
{
    Castle::Engine engine;
    int code = engine.Exec();        
    std::clog << "Return with code " << code << std::endl;
    return code;
}
