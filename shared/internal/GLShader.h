#pragma once

#include <string>
#include <glad/gl.h>

std::string readShaderFile(const std::string filename);

static void printShaderSource(const std::string text);
static int endsWith(std::string s, std::string part);

GLenum GLShaderTypeFromName(std::string filename);

void printProgramInfoLog(GLuint handle);

class GLShader
{
    public:
        explicit GLShader(const char * filename);
        GLShader(GLenum type, const char * text);
        ~GLShader();

        GLenum getType() const { return type; }
        GLuint getHandle() const { return handle; }

    private:
        GLenum type;
        GLuint handle;
};

class GLProgram
{
    public:
        GLProgram(const GLShader &a, const GLShader &b);
        GLProgram(const GLShader &a, const GLShader &b, const GLShader &c);
        GLProgram(const GLShader &a, const GLShader &b, const GLShader &c, const GLShader &d);
        GLProgram(const GLShader &a, const GLShader &b, const GLShader &c, const GLShader &d, const GLShader &e);
        GLProgram(const GLShader &a, const GLShader &b, const GLShader &c, const GLShader &d, const GLShader &e, const GLShader &f);
        ~GLProgram();

        void useProgram() const;
        GLuint gethandle() const { return handle; }

    private:
        GLuint handle;
};