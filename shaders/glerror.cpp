#include "glerror.h"

#include <sstream>
#include <GL/gl.h>

#include <stdexcept>

void CheckGLError(const std::string &function, const std::string &file, int line)
{
    GLenum error = glGetError();
    if(error != GL_NO_ERROR) {
        std::ostringstream oss;
        oss << function << ':'
            << file << ':'
            << line << ':'
            << gluErrorString(error)
            << '(' << error << ')';
        throw std::runtime_error(oss.str());
    }
}
