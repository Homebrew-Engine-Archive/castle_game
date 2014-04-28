#include "shaderprogram.h"

#include <stdexcept>

namespace Shaders
{
        
    std::string GetLinkError(GLint program)
    {
        GLint linked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if(linked == 0) {
            GLint length = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
            std::vector<GLchar> errorstr(length + 1);
            glGetProgramInfoLog(program, length, &length, &errorstr[0]);
            return std::string(errorstr.begin(), errorstr.end());
        }

        return std::string();
    }

    std::string GetCompileError(GLint shader)
    {
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if(compiled == 0) {
            GLint length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::vector<GLchar> errorstr(length + 1);
            glGetShaderInfoLog(shader, length, &length, &errorstr[0]);
            return std::string(errorstr.begin(), errorstr.end());
        }
        return std::string();
    }
    
    GLuint CompileShader(GLenum type, const std::vector<GLchar> &data)
    {
        GLint length = data.size();
        GLuint shader = glCreateShader(type);

        const GLchar *plainStr = &data[0];
        
        glShaderSource(shader, 1, &plainStr, &length);
        glCompileShader(shader);

        std::string compileError = GetCompileError(shader);
        if(!compileError.empty()) {
            throw std::runtime_error(compileError);
        }
        
        return shader;
    }

    GLuint LinkProgram(std::vector<GLuint> shaders)
    {
        GLuint program = glCreateProgram();
        for(GLuint shader : shaders) {
            glAttachShader(program, shader);
        }
        glLinkProgram(program);
        std::string linkError = Shaders::GetLinkError(program);
        if(!linkError.empty()) {
            throw std::runtime_error(linkError);
        }
        return program;
    }
}
