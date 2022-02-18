#include <shared/internal/GLShader.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <assert.h>

std::string readShaderFile(std::string filename)
{
    std::ifstream file;
    file.open(filename, std::ios::in);

    if(!file.is_open())
    {
        std::cout << "[IO ERROR]: Cannot open file " << filename << "\n";
        return std::string();
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string code(buffer.str());

    while(code.find("#include ") != code.npos)
    {
        const auto pos = code.find("#include ");
        const auto p1 = code.find('<', pos);
        const auto p2 = code.find('>', pos);

        if (p1 == code.npos || p2 == code.npos || p2 <= p1)
        {
            std::cout << "Error while loading shader program " << code.c_str() << "\n";
            return std::string();
        }

        const std::string importName = code.substr(p1 + 1, p2 - p1 - 1);
        const std::string includedFile = readShaderFile(importName);

        code.replace(pos, p2 - pos + 1, includedFile);
    }

    return code;
}

static void printShaderSource(const std::string text)
{
    std::stringstream ss(text);
    std::string line;

    int i = 1;

    while(std::getline(ss, line, '\n'))
    {
        std::cout << "(" << i << ") " << line << "\n";
        ++i;
    }
}

static int endsWith(std::string s, std::string part)
{
    if (s.length() >= part.length()) {
        return (s.compare(s.length() - part.length(), part.length(), part) == 0);
    } else {
        return false;
    } 
}

GLenum GLShaderTypeFromName(std::string filename)
{
    if(endsWith(filename, ".vert"))
        return GL_VERTEX_SHADER;
    
    if(endsWith(filename, ".frag"))
        return GL_FRAGMENT_SHADER;
    
    if(endsWith(filename, ".geom"))
        return GL_GEOMETRY_SHADER;

    if(endsWith(filename, ".tesc"))
        return GL_TESS_CONTROL_SHADER;

    if(endsWith(filename, ".tese"))
        return GL_TESS_EVALUATION_SHADER;

    if(endsWith(filename, ".comp"))
        return GL_COMPUTE_SHADER;

    assert(false);
    return 0;
}

std::string shaderTypeNameFromEnum(GLenum shaderType)
{
    switch (shaderType)
    {
    case GL_VERTEX_SHADER:
        return "Vertex Shader";

    case GL_FRAGMENT_SHADER:
        return "Fragment Shader";

    case GL_GEOMETRY_SHADER:
        return "Geometry Shader";

    case GL_TESS_CONTROL_SHADER:
        return "Tessellation Control Shader";

    case GL_TESS_EVALUATION_SHADER:
        return "Tessellation Evaluation Shader";
    
    case GL_COMPUTE_SHADER:
        return "Compute Shader";

    default:
        return "Unknown Shader Type";
    };
}

void printProgramInfoLog(GLuint handle)
{
    char buffer[8192];
    GLsizei length = 0;

    glGetProgramInfoLog(handle, sizeof(buffer), &length, buffer);
    if(length)
    {
        std::cout << buffer << "\n";
        assert(false);
    }
}

GLShader::GLShader(GLenum shaderType, const char * text)
    :
        type(shaderType)
        {
            handle = glCreateShader(type);
            glShaderSource(handle, 1, &text, nullptr);
            glCompileShader(handle);

            char buffer[8192];
            GLsizei length = 0;

            glGetShaderInfoLog(handle, sizeof(buffer), &length, buffer);
            if (length)
            {
                std::cout << "Error in " << shaderTypeNameFromEnum(shaderType) << "\n";
                std::cout << buffer << "\n";
                printShaderSource(text);
                assert(false);
            }
}

GLShader::GLShader(const char * filename)
    : GLShader(GLShaderTypeFromName(filename), readShaderFile(filename).c_str())
{
}

GLShader::~GLShader()
{
    glDeleteShader(handle);
}

GLProgram::GLProgram(const GLShader &a, const GLShader &b)
    : handle(glCreateProgram())
{
    glAttachShader(handle, a.getHandle());
    glAttachShader(handle, b.getHandle());
    glLinkProgram(handle);
    printProgramInfoLog(handle);
}

GLProgram::GLProgram(const GLShader &a, const GLShader &b, const GLShader &c)
    : handle(glCreateProgram())
{
    glAttachShader(handle, a.getHandle());
    glAttachShader(handle, b.getHandle());
    glAttachShader(handle, c.getHandle());
    glLinkProgram(handle);
    printProgramInfoLog(handle);
}

GLProgram::GLProgram(const GLShader &a, const GLShader &b, const GLShader &c, const GLShader &d)
    : handle(glCreateProgram())
{
    glAttachShader(handle, a.getHandle());
    glAttachShader(handle, b.getHandle());
    glAttachShader(handle, c.getHandle());
    glAttachShader(handle, d.getHandle());
    glLinkProgram(handle);
    printProgramInfoLog(handle);
}

GLProgram::GLProgram(const GLShader &a, const GLShader &b, const GLShader &c, const GLShader &d, const GLShader &e)
    : handle(glCreateProgram())
{
    glAttachShader(handle, a.getHandle());
    glAttachShader(handle, b.getHandle());
    glAttachShader(handle, c.getHandle());
    glAttachShader(handle, d.getHandle());
    glAttachShader(handle, e.getHandle());
    glLinkProgram(handle);
    printProgramInfoLog(handle);
}

GLProgram::GLProgram(const GLShader &a, const GLShader &b, const GLShader &c, const GLShader &d, const GLShader &e, const GLShader &f)
    : handle(glCreateProgram())
{
    glAttachShader(handle, a.getHandle());
    glAttachShader(handle, b.getHandle());
    glAttachShader(handle, c.getHandle());
    glAttachShader(handle, d.getHandle());
    glAttachShader(handle, e.getHandle());
    glAttachShader(handle, f.getHandle());
    glLinkProgram(handle);
    printProgramInfoLog(handle);
}

GLProgram::~GLProgram()
{
    glDeleteProgram(handle);
}

void GLProgram::useProgram() const
{
    glUseProgram(handle);
}
