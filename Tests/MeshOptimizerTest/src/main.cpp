#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/ext.hpp>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include <meshoptimizer.h>

static const char* shaderCodeVertex = R"(
#version 460 core
layout(std140, binding = 0) uniform PerFrameData
{
	uniform mat4 MVP;
};
layout (location=0) in vec3 pos;
layout (location=0) out vec3 color;
void main()
{
	gl_Position = MVP * vec4(pos, 1.0);
	color = pos.xyz;
}
)";

static const char* shaderCodeGeometry = R"(
#version 460 core
layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;
layout (location=0) in vec3 color[];
layout (location=0) out vec3 colors;
layout (location=1) out vec3 barycoords;
void main()
{
	const vec3 bc[3] = vec3[] (
        vec3(1.0, 0.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, 0.0, 1.0)
    );

    for (int i=0; i<3; ++i)
    {
        gl_Position = gl_in[i].gl_Position;
        colors = color[i];
        barycoords = bc[i];
        EmitVertex();
    }
    EndPrimitive();
};
)";

static const char* shaderCodeFragment = R"(
#version 460 core
layout (location=0) in vec3 colors;
layout (location=1) in vec3 barycoords;
layout (location=0) out vec4 out_FragColor;

float edgeFactor(float thickness) {
    vec3 a3 = smoothstep(vec3(0.0), fwidth(barycoords) * thickness, barycoords);
    return min(min(a3.x, a3.y), a3.z);
}

void main()
{
	out_FragColor = vec4(mix(vec3(0.0), colors, edgeFactor(1.0)), 1.0);
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

    const GLuint shaderGeometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(shaderGeometry, 1, &shaderCodeGeometry, nullptr);
    glCompileShader(shaderVertex);

    const GLuint shaderFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shaderFragment, 1, &shaderCodeFragment, nullptr);
    glCompileShader(shaderFragment);

    const GLuint program = glCreateProgram();
    glAttachShader(program, shaderVertex);
    glAttachShader(program, shaderGeometry);
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
    std::vector<unsigned int> indices;

    const aiMesh * mesh = scene->mMeshes[0];
    for(int i=0; i!= mesh->mNumVertices; ++i) {
        const aiVector3D v = mesh->mVertices[i];
        positions.emplace_back(v.x, v.z, v.y);
    }
    for(int i=0; i!= mesh->mNumFaces; ++i) {
        for(int j=0; j!=3; ++j) {
            indices.push_back(mesh->mFaces[i].mIndices[j]);
        }
    }
    aiReleaseImport(scene);

    // REMAP MESH WITH MESHOPTIMIZER
    std::vector<unsigned int> remap(indices.size());
    const size_t vertexCount = meshopt_generateVertexRemap(remap.data(), indices.data(),
                                                           indices.size(),positions.data(),
                                                           indices.size(), sizeof(glm::vec3));

    std::vector<unsigned int> remappedIndices(indices.size());
    std::vector<glm::vec3> remappedVertices(vertexCount);
    meshopt_remapIndexBuffer(remappedIndices.data(),indices.data(),
                             indices.size(), remap.data());

    meshopt_remapVertexBuffer(remappedVertices.data(), positions.data(), positions.size(),
                              sizeof(glm::vec3), remap.data());

    meshopt_optimizeVertexCache(remappedIndices.data(),remappedIndices.data(),
                                indices.size(), vertexCount);

    meshopt_optimizeOverdraw(remappedIndices.data(), remappedIndices.data(), indices.size(),
                             glm::value_ptr(remappedVertices[0]), vertexCount,
                             sizeof(glm::vec3), 1.05f);

    const float threshold = 0.2f;
    const auto target_index_count = size_t(remappedIndices.size() * threshold);
    const float target_error = 1e-2f;
    std::vector<unsigned int> indicesLod(remappedIndices.size());
    indicesLod.resize(meshopt_simplify(&indicesLod[0], remappedIndices.data(),
                                       remappedIndices.size(),&remappedVertices[0].x,
                                       vertexCount, sizeof(glm::vec3),
                                       target_index_count, target_error));

    indices = remappedIndices;
    positions = remappedVertices;

    // CREATING BUFFERS
    const size_t sizeIndices = sizeof(unsigned int) * indices.size();
    const size_t sizeIndicesLod = sizeof(unsigned int) * indices.size();
    const size_t sizeVertices = sizeof(glm::vec3) * positions.size();

    GLuint meshData;
    glCreateBuffers(1, &meshData);
    glNamedBufferStorage(meshData, sizeIndices + sizeIndicesLod + sizeVertices, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferSubData(meshData, 0, sizeIndices, indices.data());
    glNamedBufferSubData(meshData, sizeIndices, sizeIndicesLod, indicesLod.data());
    glNamedBufferSubData(meshData, sizeIndices + sizeIndicesLod, sizeVertices, positions.data());

    glVertexArrayElementBuffer(VAO, meshData);
    glVertexArrayVertexBuffer(VAO, 0, meshData, sizeIndices + sizeIndicesLod, sizeof(glm::vec3));
    glEnableVertexArrayAttrib(VAO, 0);
    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(VAO, 0, 0);

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
                glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, -2.0f)),
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
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

        perFrameData.mvp = p * glm::rotate(
                glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -0.5f, -2.0f)),
                (float) glfwGetTime(),
                glm::vec3(0.0f, 1.0f, 0.0f));

        glNamedBufferSubData(perFrameDataBuf, 0, kBufferSize, &perFrameData);
        glDrawElements(GL_TRIANGLES, indicesLod.size(), GL_UNSIGNED_INT, (void*) sizeIndices);

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
