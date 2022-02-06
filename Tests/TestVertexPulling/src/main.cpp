#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/version.h>

#include <shared/internal/GLShader.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using glm::mat4;
using glm::vec3;
using glm::vec2;

struct VertexData
{
    vec3 pos;
    vec2 tc;
};

struct PerFrameData
{
    glm::mat4 mvp;
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
    GLShader vertexShader("../Tests/TestVertexPulling/shaders/test1.vert");
    GLShader fragmentShader("../Tests/TestVertexPulling/shaders/test1.frag");
    GLShader geometryShader("../Tests/TestVertexPulling/shaders/test1.geom");

    GLProgram program(vertexShader, fragmentShader, geometryShader);
    program.useProgram();

    // LOADING ASSIMP GLFT2 FILE
    const aiScene * scene = aiImportFile("../data/rubber_duck/scene.gltf", aiProcess_Triangulate);
    if (!scene || !scene->HasMeshes()) {
        std::cout << "ASSIMP ERROR: Unable to load file.\n";
        exit(255);
    }

    const aiMesh *mesh = scene->mMeshes[0];
    std::vector<VertexData> vertices;
    for (int i = 0; i!=mesh->mNumVertices; ++i)
    {
        const aiVector3D v = mesh->mVertices[i];
        const aiVector3D t = mesh->mTextureCoords[0][i];
        vertices.push_back({
            .pos = vec3(v.x, v.z, v.y),
            .tc = vec2(t.x, t.y)});
    }

    std::vector<unsigned int> indices;
    for (int i = 0; i != mesh->mNumFaces; ++i)
    {
        for (int j = 0; j != 3; ++j)
        {
            indices.push_back(mesh->mFaces[i].mIndices[j]);
        }
    }

    aiReleaseImport(scene);

    // CREATING BUFFERS
    const size_t kSizeIndices = sizeof(unsigned int) * indices.size();
    const size_t kSizeVertices = sizeof(VertexData) * vertices.size();
    GLuint dataIndices;
    GLuint dataVertices;
    glCreateBuffers(1, &dataIndices);
    glCreateBuffers(1, &dataVertices);
    glNamedBufferStorage(dataIndices, kSizeIndices, indices.data(), 0);
    glNamedBufferStorage(dataVertices, kSizeVertices, vertices.data(), 0);

    glVertexArrayElementBuffer(VAO, dataIndices);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, dataVertices);

    // CREATING MATRIX BUFFER DATA
    const GLsizeiptr kBufferSize = sizeof(PerFrameData);
    GLuint perFrameDataBuf;
    glCreateBuffers(1, &perFrameDataBuf);
    glNamedBufferStorage(perFrameDataBuf, kBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, perFrameDataBuf, 0, kBufferSize);

    // ENABLING THINGS
    glEnable(GL_DEPTH_TEST);

    // LOADING TEXTURE
    int w, h, comp;
    const uint8_t *img = stbi_load("../data/rubber_duck/textures/Duck_baseColor.png", &w, &h, &comp, 3);
    GLuint tx;
    glCreateTextures(GL_TEXTURE_2D, 1, &tx);
    glTextureParameteri(tx, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(tx, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureStorage2D(tx, 1, GL_RGB8, w, h);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTextureSubImage2D(tx, 0, 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, img);
    glBindTextures(0, 1, &tx);

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
                .mvp = p * m
        };

        // SENDING BUFFER AND DRAW CUBE
        glNamedBufferSubData(perFrameDataBuf, 0, kBufferSize, &perFrameData);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
