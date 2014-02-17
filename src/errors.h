#ifndef ERRORS_H_
#define ERRORS_H_

#include <sstream>
#include <exception>

#include <SDL2/SDL.h>

#define DEFINE_ERROR_SUBTYPE(name, super)                               \
    struct name : public super                                          \
    {                                                                   \
        const char * what_arg;                                          \
        explicit name(const char *what_arg)                             \
            throw() : what_arg(what_arg)                                \
        {}                                                              \
        virtual const char * what() const                               \
            throw()                                                     \
        {                                                               \
            return what_arg;                                            \
        }                                                               \
    }                                                                   \

#define DEFINE_ERROR_TYPE(name) DEFINE_ERROR_SUBTYPE(name, std::exception)

DEFINE_ERROR_TYPE(SDLError);
DEFINE_ERROR_TYPE(IOError);

#endif
