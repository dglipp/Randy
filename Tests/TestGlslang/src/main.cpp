#define VK_NO_PROTOTYPES
#define GLFW_INCLUDE_VULKAN

#include <fstream>

#include <shared/internal/UtilsVulkan.h>

void saveSPIRVBinaryFile(const char* filename, unsigned int* code, size_t size)
{
    std::ofstream f(filename, std::ios::out | std::ios::binary);
    
    if(!f)
        return;

    f.write((char *)code, size * sizeof(uint32_t));
    f.close();
}

void testShaderCompilation(std::string sourceFilename, const char* destFilename)
{
    ShaderModule shaderModule;
    if(compileShaderFile(sourceFilename, shaderModule) < 1)
        return;
    saveSPIRVBinaryFile(destFilename, shaderModule.SPIRV.data(), shaderModule.SPIRV.size());
}

int main()
{
    glslang_initialize_process();
    testShaderCompilation("../Tests/TestGlslang/shaders/VK.vert", "../Tests/TestGlslang/shaders/VK.vrt.bin");
    testShaderCompilation("../Tests/TestGlslang/shaders/VK.frag", "../Tests/TestGlslang/shaders/VK.frg.bin");
    glslang_finalize_process();
    return 0;
}
