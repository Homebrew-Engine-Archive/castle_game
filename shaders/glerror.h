#ifndef GLERROR_H_
#define GLERROR_H_

#include <string>
#include <boost/current_function.hpp>
#include <GL/glu.h>

#define CHECK_GL_ERROR do {                                             \
                           CheckGLError(BOOST_CURRENT_FUNCTION, __FILE__, __LINE__); \
                           } while(0)


void CheckGLError(const std::string &function, const std::string &file, int line);

#endif // GLERROR_H_
