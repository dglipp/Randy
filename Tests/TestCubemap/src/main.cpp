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
#include <shared/internal/Bitmap.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::ivec2;

struct VertexData
{
    vec3 pos;
    vec3 n;
    vec2 tc;
};

struct PerFrameData
{
    mat4 model;
    mat4 mvp;
    vec4 cameraPos;
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

    GLFWwindow * window = glfwCreateWindow(1024, 768, "Cubemap example", nullptr, nullptr);
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
    GLShader vertexShaderModel("../Tests/TestCubemap/shaders/duck.vert");
    GLShader fragmentShaderModel("../Tests/TestCubemap/shaders/duck.frag");
    GLProgram programModel(vertexShaderModel, fragmentShaderModel);

    GLShader vertexShaderCubemap("../Tests/TestCubemap/shaders/cubemap.vert");
    GLShader fragmentShaderCubemap("../Tests/TestCubemap/shaders/cubemap.frag");
    GLProgram programCubemap(vertexShaderCubemap, fragmentShaderCubemap);

    // LOADING ASSIMP GLFT2 FILE
    const aiScene *scene = aiImportFile("../data/rubber_duck/scene.gltf", aiProcess_Triangulate);
    if (!scene || !scene->HasMeshes()) {
        std::cout << "ASSIMP ERROR: Unable to load file.\n";
        exit(255);
    }

    const aiMesh *mesh = scene->mMeshes[0];
    std::vector<VertexData> vertices;
    for (int i = 0; i!=mesh->mNumVertices; ++i)
    {
        const aiVector3D v = mesh->mVertices[i];
        const aiVector3D n = mesh->mNormals[i];
        const aiVector3D t = mesh->mTextureCoords[0][i];
        vertices.push_back({
            .pos = vec3(v.x, v.z, v.y),
            .n = vec3(n.x, n.y, n.z),
            .tc = vec2(t.x, t.y) });
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

    // LOADING HDR IMAGE
    GLuint cubemapTex;
	{
		int w, h, comp;
		const float* img = stbi_loadf("../data/piazza_bologni_1k.hdr", &w, &h, &comp, 3);
		Bitmap in(w, h, comp, eBitmapFormat_Float, img);
		Bitmap out = convertEquirectangularMapToVerticalCross(in);
		stbi_image_free((void*)img);

		// stbi_write_hdr("screenshot.hdr", out.w_, out.h_, out.comp_, (const float*)out.data_.data());

        Bitmap cubemap = convertVerticalCrossToCubeMapFaces(out);

        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &cubemapTex);
        glTexParameteri(cubemapTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(cubemapTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(cubemapTex, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTextureParameteri(cubemapTex, GL_TEXTURE_BASE_LEVEL, 0);
		glTextureParameteri(cubemapTex, GL_TEXTURE_MAX_LEVEL, 0);
		glTextureParameteri(cubemapTex, GL_TEXTURE_MAX_LEVEL, 0);
		glTextureParameteri(cubemapTex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(cubemapTex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTextureStorage2D(cubemapTex, 1, GL_RGB32F, cubemap.w_, cubemap.h_);
        const uint8_t *data = cubemap.data_.data();

        for (int i = 0; i != 6; ++i)
        {
            glTextureSubImage3D(cubemapTex, 0, 0, 0, i, cubemap.w_, cubemap.h_, 1, GL_RGB, GL_FLOAT, data);
            data += cubemap.w_ * cubemap.h_ * cubemap.comp_ * Bitmap::getBytesPerComponents(cubemap.fmt_);
        }
        glBindTextures(1, 1, &cubemapTex);
    }

    // MAIN LOOP
    while(!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // MVP MATRIX
        const float ratio = width / (float) height;


        mat4 m = glm::rotate(
                translate(mat4(1.0f), vec3(0.0f, -0.5f, -1.5f)),
                (float) glfwGetTime(),
                vec3(0.0f, 1.0f, 0.0f));

        const mat4 p = glm::perspective(glm::radians(45.0f), ratio, 0.1f, 1000.0f);

        PerFrameData perFrameData = {
            .model = m,
            .mvp = p * m,
            .cameraPos = vec4(0.0f)};

        // DRAW MODEL
        glNamedBufferSubData(perFrameDataBuf, 0, kBufferSize, &perFrameData);
        programModel.useProgram();
        glDrawElements(GL_TRIANGLES, static_cast<unsigned>(indices.size()), GL_UNSIGNED_INT, nullptr);

        // DRAW CUBEMAP
        m = glm::scale(mat4(1.0f), vec3(2.0f));
        perFrameData.model = m;
        perFrameData.mvp = p * m;
        glNamedBufferSubData(perFrameDataBuf, 0, kBufferSize, &perFrameData);
        programCubemap.useProgram();
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &dataIndices);
    glDeleteBuffers(1, &dataVertices);
    glDeleteBuffers(1, &perFrameDataBuf);
    glDeleteVertexArrays(1, &VAO);

    return 0;
}
