#ifndef SHADERPROGRAM_H_
#define SHADERPROGRAM_H_

#include <vector>
#include <string>
#include <GL/glew.h>
#include <GL/gl.h>

namespace Shaders
{
    std::string GetLinkError(GLint program);
    std::string GetCompileError(GLint shader);
    GLuint CompileShader(GLenum type, const std::vector<GLchar> &data);
    GLuint LinkProgram(std::vector<GLuint> shaders);
}

#endif // SHADERPROGRAM_H_
