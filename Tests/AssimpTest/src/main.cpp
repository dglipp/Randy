#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/version.h>

static const char* shaderCodeVertex = R"(
#version 460 core
layout(std140, binding = 0) uniform PerFrameData
{
	uniform mat4 MVP;
	uniform int isWireframe;
};
layout (location=0) in vec3 pos;
layout (location=0) out vec3 color;
void main()
{
	gl_Position = MVP * vec4(pos, 1.0);
	color = isWireframe > 0 ? vec3(0.0f) : pos.xyz;
}
)";

static const char* shaderCodeFragment = R"(
#version 460 core
layout (location=0) in vec3 color;
layout (location=0) out vec4 out_FragColor;
void main()
{
	out_FragColor = vec4(color, 1.0);
};
)";

struct PerFrameData {
    glm::mat4 mvp;
    int isWireframe;
};

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

    // LOADING ASSIMP GLFT2 FILE
    const aiScene * scene = aiImportFile("../data/rubber_duck/scene.gltf", aiProcess_Triangulate);
    if (!scene || !scene->HasMeshes()) {
        std::cout << "ASSIMP ERROR: Unable to load file.\n";
        exit(255);
    }

    std::vector<glm::vec3> positions;
    const aiMesh * mesh = scene->mMeshes[0];
    for(int i=0; i!= mesh->mNumFaces; ++i) {
        const aiFace & face = mesh->mFaces[i];
        const unsigned int idx[3] = { face.mIndices[0], face.mIndices[1], face.mIndices[2] };
        for(unsigned int j : idx) {
            const aiVector3D v = mesh->mVertices[j];
            positions.emplace_back(v.x, v.z, v.y);
        }
    }
    aiReleaseImport(scene);

    // CREATING MODEL BUFFER DATA
    GLuint meshData;
    glCreateBuffers(1, &meshData);
    glNamedBufferStorage(meshData, sizeof(glm::vec3) * positions.size(), positions.data(), 0);
    glVertexArrayVertexBuffer(VAO, 0, meshData, 0, sizeof(glm::vec3));
    glEnableVertexArrayAttrib(VAO, 0);
    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(VAO, 0, 0);
    const int numVertices = static_cast<int>(positions.size());

    // CREATING MATRIX BUFFER DATA
    const GLsizeiptr kBufferSize = sizeof(PerFrameData);
    GLuint perFrameDataBuf;
    glCreateBuffers(1, &perFrameDataBuf);
    glNamedBufferStorage(perFrameDataBuf, kBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, perFrameDataBuf, 0, kBufferSize);

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
        const glm::mat4 m = glm::rotate(
                glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, -2.0f)),
                (float) glfwGetTime(),
                glm::vec3(0.0f, 1.0f, 0.0f));

        const glm::mat4 p = glm::perspective(glm::radians(45.0f), ratio, 0.1f, 1000.0f);

        // BUFFER VARIABLE
        PerFrameData perFrameData = {
                .mvp = p * m,
                .isWireframe = false
        };

        // SENDING BUFFER AND DRAW CUBE
        glNamedBufferSubData(perFrameDataBuf, 0, kBufferSize, &perFrameData);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLES, 0, numVertices);

        // SENDING BUFFER AND DRAW LINES
        perFrameData.isWireframe = true;

        glNamedBufferSubData(perFrameDataBuf, 0, kBufferSize, &perFrameData);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, numVertices);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // CLEAN UP
    glDeleteProgram(program);
    glDeleteShader(shaderVertex);
    glDeleteShader(shaderFragment);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &meshData);
    glDeleteBuffers(1, &perFrameDataBuf);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
