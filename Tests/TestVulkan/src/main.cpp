#define VK_NO_PROTOTYPES
#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include <cstring>

#include <glm/glm.hpp>

#include <shared/internal/UtilsVulkan.h>

static constexpr VkClearColorValue clearValueColor = {1.0f, 1.0f, 1.0f, 1.0f};
const uint32_t kScreenWidth = 1280;
const uint32_t kScreenHeight = 720;
size_t vertexBufferSize;
size_t indexBufferSize;

VulkanInstance vk;
VulkanRenderDevice vkDev;

using glm::mat4;

struct VulkanState
{
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<VkFramebuffer> swapchainFramebuffers;

    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;

    VkBuffer storageBuffer;
    VkDeviceMemory storageBufferMemory;

    VulkanImage depthTexture;

    VkSampler textureSampler;
    VulkanImage texture;
} vkState;

struct UniformBuffer
{
    mat4 mvp;
} ubo;

bool fillCommandBuffers(size_t i)
{
    const VkCommandBufferBeginInfo bi = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
        .pInheritanceInfo = nullptr};

    const std::array<VkClearValue, 2> clearValues = {
        VkClearValue{.color = clearValueColor},
        VkClearValue{.depthStencil = {1.0f, 0}}};

    const VkRect2D screenRect = {
        .offset = {0, 0},
        .extent = {.width = kScreenWidth, .height = kScreenHeight}};

    VK_CHECK(vkBeginCommandBuffer(vkDev.commandBuffers[i], &bi));
    const VkRenderPassBeginInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = vkState.renderPass,
        .framebuffer = vkState.swapchainFramebuffers[i],
        .renderArea = screenRect,
        .clearValueCount = static_cast<uint32_t>(clearValues.size()),
        .pClearValues = clearValues.data()};
    vkCmdBeginRenderPass(vkDev.commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(vkDev.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkState.graphicsPipeline);
    vkCmdBindDescriptorSets(vkDev.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkState.pipelineLayout, 0, 1, &vkState.descriptorSets[i], 0, nullptr);
    vkCmdDraw(vkDev.commandBuffers[i], static_cast<uint32_t>(indexBufferSize / sizeof(uint32_t)), 1, 0, 0);
    vkCmdEndRenderPass(vkDev.commandBuffers[i]);
    VK_CHECK(vkEndCommandBuffer(vkDev.commandBuffers[i]));

    return true;
}

bool createUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(UniformBuffer);
    vkState.uniformBuffers.resize(vkDev.swapchainImages.size());
    vkState.uniformBuffersMemory.resize(vkDev.swapchainImages.size());

    for (size_t i = 0; i < vkDev.swapchainImages.size(); ++i)
    {
        if (!createBuffer(vkDev.device, vkDev.physicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vkState.uniformBuffers[i], vkState.uniformBuffersMemory[i]))
        {
            std::cout << "Fail: buffers\n";
            return false;
        }
    }
    return true;
}

void updateUniformBuffer(uint32_t currentImage, const UniformBuffer & ubo)
{
    void *data = nullptr;
    vkMapMemory(vkDev.device, vkState.uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
    std::memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(vkDev.device, vkState.uniformBuffersMemory[currentImage]);
}

bool createDescriptorSet()
{
    const std::array<VkDescriptorSetLayoutBinding, 4> bindings = {
        descriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
        descriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
        descriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
        descriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)};

    const VkDescriptorSetLayoutCreateInfo li = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings = bindings.data()};

    VK_CHECK(vkCreateDescriptorSetLayout(vkDev.device, &li, nullptr, &vkState.descriptorSetLayout));

    std::vector<VkDescriptorSetLayout> layouts(vkDev.swapchainImages.size(), vkState.descriptorSetLayout);

    VkDescriptorSetAllocateInfo ai = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = vkState.descriptorPool,
        .descriptorSetCount = static_cast<uint32_t>(vkDev.swapchainImages.size()),
        .pSetLayouts = layouts.data()};

    vkState.descriptorSets.resize(vkDev.swapchainImages.size());

    VK_CHECK(vkAllocateDescriptorSets(vkDev.device, &ai, vkState.descriptorSets.data()));

    for (size_t i = 0; i < vkDev.swapchainImages.size(); ++i)
    {
        VkDescriptorBufferInfo bi1 = {
            .buffer = vkState.uniformBuffers[i],
            .offset = 0,
            .range = sizeof(UniformBuffer)};

        VkDescriptorBufferInfo bi2 = {
            .buffer = vkState.storageBuffer,
            .offset = 0,
            .range = vertexBufferSize};

        VkDescriptorBufferInfo bi3 = {
            .buffer = vkState.storageBuffer,
            .offset = vertexBufferSize,
            .range = indexBufferSize};

        VkDescriptorImageInfo ii = {
            .sampler = vkState.textureSampler,
            .imageView = vkState.texture.imageView,
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};

        std::array<VkWriteDescriptorSet, 4> descriptorWrites = {
            VkWriteDescriptorSet{
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = vkState.descriptorSets[i],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pBufferInfo = &bi1},
            VkWriteDescriptorSet{
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = vkState.descriptorSets[i],
                .dstBinding = 1,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .pBufferInfo = &bi2},
            VkWriteDescriptorSet{
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = vkState.descriptorSets[i],
                .dstBinding = 2,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .pBufferInfo = &bi3},
            VkWriteDescriptorSet{
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = vkState.descriptorSets[i],
                .dstBinding = 1,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &ii}};

        vkUpdateDescriptorSets(vkDev.device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

        return true;
    }
}

int main()
{
}
