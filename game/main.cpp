#include <iostream>
#include <game/engine.h>

int main()
{
    std::unique_ptr<Castle::Engine> engine;

    try {
        engine.reset(new Castle::Engine);
    } catch(const std::exception &error) {
        std::cerr << "Init failed: " << error.what() << std::endl;
        throw;
    } catch(...) {
        std::cerr << "Unhandled exception during init" << std::endl;
        throw;
    }

    int code = engine->Exec();        
    std::clog << "Return with code " << code << std::endl;
    return code;
}
