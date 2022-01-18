#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/ext.hpp>

static const char * shaderCodeVertex = R"(
        #version 460 core
        layout(std140, binding = 0) uniform PerFrameData
        {
            uniform mat4 MVP;
        };
        layout (location=0) out vec2 uv;
        const vec2 pos[3] = vec2[3](
            vec2(-0.6f, -0.4f),
            vec2( 0.6f, -0.4f),
            vec2( 0.0f,  0.6f)
        );
        const vec2 tc[3] = vec2[3](
            vec2( 0.0, 0.0 ),
            vec2( 1.0, 0.0 ),
            vec2( 0.5, 1.0 )
        );
        void main()
        {
            gl_Position = MVP * vec4(pos[gl_VertexID], 0.0, 1.0);
            uv = tc[gl_VertexID];
        }
)";

static const char * shaderCodeFragment = R"(
        #version 460 core
        layout (location=0) in vec2 uv;
        layout (location=0) out vec4 out_FragColor;
        uniform sampler2D texture0;

        void main() {
            out_FragColor = texture(texture0, uv);
        };
    )";

int main()
{
    // INIT

    glfwSetErrorCallback([] (int error, const char * description) {
       std::cout << "Error: " << description << "\n";
    });

    if(!glfwInit()) exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow * window = glfwCreateWindow(1024, 768, "First example", nullptr, nullptr);
    if(!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, [] (GLFWwindow * window, int key, int scancode, int action, int mods) {
       if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
       if(key == GLFW_KEY_HOME && action == GLFW_PRESS) {
           int width, height;
           glfwGetFramebufferSize(window, &width, &height);
           auto * ptr = new uint8_t[width * height * 4];
           glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, ptr);
           stbi_write_png("screenshot.png", width, height, 4, ptr, 0);
           delete[] ptr;
       }
    });

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    // CREATING VAO
    GLuint VAO;
    glCreateVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // COMPILING SHADERS
    const GLuint shaderVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shaderVertex, 1, &shaderCodeVertex, nullptr);
    glCompileShader(shaderVertex);

    const GLuint shaderFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shaderFragment, 1, &shaderCodeFragment, nullptr);
    glCompileShader(shaderFragment);

    const GLuint program = glCreateProgram();
    glAttachShader(program, shaderVertex);
    glAttachShader(program, shaderFragment);
    glLinkProgram(program);
    glUseProgram(program);

    // CREATING BUFFER DATA
    const GLsizeiptr kBufferSize = sizeof(glm::mat4);
    GLuint perFrameDataBuf;
    glCreateBuffers(1, &perFrameDataBuf);
    glNamedBufferStorage(perFrameDataBuf, kBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, perFrameDataBuf, 0, kBufferSize);

    // LOADING TEXTURE
    int w, h, comp;
    const uint8_t* img = stbi_load( "/home/dglipp/Projects/Randy/data/ch2_sample3_STB.jpg", &w, &h, &comp, 3 );
    GLuint texture;
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    glTextureParameteri(texture, GL_TEXTURE_MAX_LEVEL, 0);
    glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureStorage2D(texture, 1, GL_RGB8, w, h);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTextureSubImage2D(texture, 0, 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, img);
    glBindTextures(0, 1, &texture);

    stbi_image_free((void *)img);

    // ENABLING THINGS
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.0f, -1.0f);

    // MAIN LOOP
    while(!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // MVP MATRIX
        const float ratio = width / (float) height;
        const glm::mat4 m = glm::rotate(glm::mat4(1.0f),
                                        (float) glfwGetTime(),
                                        glm::vec3(1.0f, 1.0f, 1.0f));

        const glm::mat4 p = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 1.0f, -1.0f);

        const glm::mat4 mvp = p * m;

        // SENDING BUFFER AND DRAW
        glNamedBufferSubData(perFrameDataBuf, 0, kBufferSize, glm::value_ptr(mvp));
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // CLEAN UP
    glDeleteProgram(program);
    glDeleteShader(shaderVertex);
    glDeleteShader(shaderFragment);
    glDeleteVertexArrays(1, &VAO);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
