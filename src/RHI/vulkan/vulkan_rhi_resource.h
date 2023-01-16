//
// Created by arrayJY on 2022/12/19.
//

#ifndef SPARROWENGINE_VULKAN_RHI_RESOURCE_H
#define SPARROWENGINE_VULKAN_RHI_RESOURCE_H

#include <vector>
#include "RHI/rhi_struct.h"
#include "vulkan/vulkan.hpp"

#define RESOURCE_COMMON_BODY(TYPE)               \
 public:                                         \
  void setResource(TYPE res) { resource = res; } \
  TYPE getResource() const { return resource; }  \
  TYPE& getResourceRef() { return resource; }    \
                                                 \
 private:                                        \
  TYPE resource;

#define DEF_RESOURCE_CLASS(ClassName, ParentName, ResourceType) \
  class ClassName : public ParentName {                         \
   public:                                                      \
    using Type = ResourceType;                                  \
    ClassName() : resource() {}                                 \
    ClassName(const ResourceType& res) : resource(res) {}       \
    RESOURCE_COMMON_BODY(ResourceType)                          \
  };

#define DEF_VULKAN_RESOURCE_CLASS(ClassBaseName, ResourceType) \
  DEF_RESOURCE_CLASS(Vulkan##ClassBaseName, RHI##ClassBaseName, ResourceType);
#define DEF_VULKAN_RESOURCE_VECTOR_CLASS(ClassBaseName, ResourceType) \
  DEF_RESOURCE_CLASS(Vulkan##ClassBaseName##s, RHI##ClassBaseName##s, \
                     std::vector<ResourceType>);

namespace Sparrow {
DEF_VULKAN_RESOURCE_CLASS(Pipeline, vk::Pipeline);
DEF_VULKAN_RESOURCE_CLASS(PipelineLayout, vk::PipelineLayout);
DEF_VULKAN_RESOURCE_CLASS(Shader, vk::ShaderModule);
DEF_VULKAN_RESOURCE_CLASS(RenderPass, vk::RenderPass);
DEF_VULKAN_RESOURCE_CLASS(CommandBuffer, vk::CommandBuffer);
DEF_VULKAN_RESOURCE_CLASS(Framebuffer, vk::Framebuffer);
DEF_VULKAN_RESOURCE_CLASS(ImageView, vk::ImageView);
DEF_VULKAN_RESOURCE_CLASS(Buffer, vk::Buffer);
DEF_VULKAN_RESOURCE_CLASS(Image, vk::Image);
DEF_VULKAN_RESOURCE_CLASS(BufferView, vk::BufferView);
DEF_VULKAN_RESOURCE_CLASS(DeviceMemory, vk::DeviceMemory);
DEF_VULKAN_RESOURCE_CLASS(DescriptorSet, vk::DescriptorSet);
DEF_VULKAN_RESOURCE_VECTOR_CLASS(DescriptorSet, vk::DescriptorSet);
DEF_VULKAN_RESOURCE_CLASS(DescriptorSetLayout, vk::DescriptorSetLayout);
DEF_VULKAN_RESOURCE_CLASS(DescriptorPool, vk::DescriptorPool);
DEF_VULKAN_RESOURCE_CLASS(Sampler, vk::Sampler);

}  // namespace Sparrow

#endif