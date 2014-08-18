#include "engine.h"
#include <exception>

int main(int argc, const char *argv[])
{
    try {
        gmtool::Engine engine;
        return engine.Exec(argc, argv);
    } catch(const std::exception &error) {
        std::cerr << error.what() << std::endl;
        return EXIT_FAILURE;
    }
}
