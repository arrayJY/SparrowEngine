//
// Created by arrayJY on 2022/12/14.
//

#ifndef SPARROWENGINE_RHI_STRUCT_H
#define SPARROWENGINE_RHI_STRUCT_H

#include <array>
#include <cstdint>
#include "function/render_enum.h"

// #include <vulkan/vulkan.hpp>

namespace Sparrow {

using RHIBool32 = uint32_t;
static constexpr RHIBool32 RHIFalse = 0U;
static constexpr RHIBool32 RHITrue = 1U;

using RHIDeviceSize = uint64_t;

#pragma region Command
struct RHICommandBufferInheritanceInfo;
#pragma endregion

template <typename T, std::size_t Size>
using RHIArray = std::array<T, Size>;

#pragma region Classes

class RHIPipeline {};
class RHIShader {};
class RHIPipelineLayout {};
class RHIRenderPass {};
class RHIFramebuffer {};
class RHICommandBuffer {};
class RHIImageView {};
class RHIBuffer {};
class RHIDeviceMemory {};

#pragma endregion

#pragma region Pipeline

#pragma region PipelinePreviousDeclare
struct RHIPipelineShaderStageCreateInfo;
struct RHIDynamicStateCreateInfo;
struct RHIVertexInputStateCreateInfo;
struct RHIInputAssemblyStateCreateInfo;
struct RHIViewportStateCreateInfo;
struct RHIRasterizationStateCreateInfo;
struct RHIMultisampleStateCreateInfo;
struct RHIDepthStencilStateCreateInfo;
struct RHIColorBlendStateCreateInfo;
struct RHIPipelineLayoutCreateInfo;
struct RHIRenderPassCreateInfo;

struct RHISpecializationInfo;
struct RHISpecializationMapEntry;

struct RHIVertexBindingDescription;
struct RHIVertexAttributeDescription;

struct RHIViewport;
struct RHIOffset2D;
struct RHIExtend2D;
struct RHIRect2D;

struct RHIStencilOpState;

struct RHIColorBlendAttachmentState;

struct RHIDescriptorSetLayout;
struct RHIPushConstantRange;

struct RHIAttachmentDescription;
struct RHISubpassDescription;
struct RHISubpassDependency;
struct RHIAttachmentReference;

#pragma endregion

struct RHIGraphicsPipelineCreateInfo {
  uint32_t stageCount;
  const RHIPipelineShaderStageCreateInfo* shaderStageCreateInfo;
  const RHIDynamicStateCreateInfo* dynamicStateCreateInfo;
  const RHIVertexInputStateCreateInfo* vertexInputStateCreateInfo;
  const RHIInputAssemblyStateCreateInfo* inputAssemblyStateCreateInfo;
  const RHIViewportStateCreateInfo* viewportStateCreateInfo;
  const RHIRasterizationStateCreateInfo* rasterizationStateCreateInfo;
  const RHIMultisampleStateCreateInfo* multisampleStateCreateInfo;
  const RHIDepthStencilStateCreateInfo* depthStencilStateCreateInfo;
  const RHIColorBlendStateCreateInfo* colorBlendStateCreateInfo;
  RHIPipelineLayout* pipelineLayout;
  RHIRenderPass* renderPass;
  uint32_t subpass;
  RHIPipeline* basePipelineHandle;
  int32_t basePipelineIndex;
};

struct RHIPipelineShaderStageCreateInfo {
  RHIShaderStageFlag stage = RHIShaderStageFlag::Vertex;
  RHIShader* module = {};
  const char* name = {};
  const RHISpecializationInfo* specializationInfo = {};
};
struct RHISpecializationInfo {
  uint32_t mapEntryCount = {};
  const RHISpecializationMapEntry* pMapEntries = {};
  size_t dataSize = {};
  const void* pData = {};
};
struct RHISpecializationMapEntry {
  uint32_t constantID = {};
  uint32_t offset = {};
  size_t size = {};
};

struct RHIDynamicStateCreateInfo {
  uint32_t dynamicStateCount = {};
  const RHIDynamicState* dynamicStates = {};
};
struct RHIVertexInputStateCreateInfo {
  uint32_t vertexBindingDescriptionCount;
  const RHIVertexBindingDescription* vertexBindingDescriptions;
  uint32_t vertexAttributeDescriptionCount;
  const RHIVertexAttributeDescription* vertexAttributeDescriptions;
};
struct RHIVertexBindingDescription {
  uint32_t binding = {};
  uint32_t stride = {};
  RHIVertexInputRate inputRate = RHIVertexInputRate::Vertex;
};
struct RHIVertexAttributeDescription {
  uint32_t location = {};
  uint32_t binding = {};
  RHIFormat format = RHIFormat::Undefined;
  uint32_t offset = {};
};

struct RHIInputAssemblyStateCreateInfo {
  RHIPrimitiveTopology topology;
  RHIBool32 primitiveRestartEnabled;
};

struct RHIRasterizationStateCreateInfo {
  RHIBool32 depthClampEnable = {};
  RHIBool32 rasterizerDiscardEnable = {};
  RHIPolygonMode polygonMode = RHIPolygonMode::Fill;
  RHICullMode cullMode = {};
  RHIFrontFace frontFace = RHIFrontFace::CounterClockwise;
  RHIBool32 depthBiasEnable = {};
  float depthBiasConstantFactor = {};
  float depthBiasClamp = {};
  float depthBiasSlopeFactor = {};
  float lineWidth = {};
};

struct RHIViewportStateCreateInfo {
  uint32_t viewportCount = {};
  const RHIViewport* viewports = {};
  uint32_t scissorCount = {};
  const RHIRect2D* scissors = {};
};
struct RHIViewport {
  float x = {};
  float y = {};
  float width = {};
  float height = {};
  float minDepth = {};
  float maxDepth = {};
};
struct RHIOffset2D {
  int32_t x = {};
  int32_t y = {};
};
struct RHIExtend2D {
  uint32_t width = {};
  uint32_t height = {};
};
struct RHIRect2D {
  RHIOffset2D offset = {};
  RHIExtend2D extend = {};
};

struct RHIMultisampleStateCreateInfo {
  RHISampleCount rasterizationSamples = RHISampleCount::Count1;
  RHIBool32 sampleShadingEnable = {};
  float minSampleShading = {};
  const RHISampleMask* sampleMask = {};
  RHIBool32 alphaToCoverageEnable = {};
  RHIBool32 alphaToOneEnable = {};
};

struct RHIStencilOpState {
  RHIStencilOp failOp = RHIStencilOp::Keep;
  RHIStencilOp passOp = RHIStencilOp::Keep;
  RHIStencilOp depthFailOp = RHIStencilOp::Keep;
  RHICompareOp compareOp = RHICompareOp::Never;
  uint32_t compareMask = {};
  uint32_t writeMask = {};
  uint32_t reference = {};
};

struct RHIDepthStencilStateCreateInfo {
  RHIBool32 depthTestEnable = {};
  RHIBool32 depthWriteEnable = {};
  RHICompareOp depthCompareOp = RHICompareOp::Never;
  RHIBool32 depthBoundsTestEnable = {};
  RHIBool32 stencilTestEnable = {};
  RHIStencilOpState front = {};
  RHIStencilOpState back = {};
  float minDepthBounds = {};
  float maxDepthBounds = {};
};

struct RHIColorBlendStateCreateInfo {
  RHIBool32 logicOpEnable = {};
  RHILogicOp logicOp = RHILogicOp::Clear;
  uint32_t attachmentCount = {};
  const RHIColorBlendAttachmentState* attachments = {};
  RHIArray<float, 4> blendConstants = {};
};
struct RHIColorBlendAttachmentState {
  RHIBool32 blendEnable = {};
  RHIBlendFactor srcColorBlendFactor = RHIBlendFactor::Zero;
  RHIBlendFactor dstColorBlendFactor = RHIBlendFactor::Zero;
  RHIBlendOp colorBlendOp = RHIBlendOp::Add;
  RHIBlendFactor srcAlphaBlendFactor = RHIBlendFactor::Zero;
  RHIBlendFactor dstAlphaBlendFactor = RHIBlendFactor::Zero;
  RHIBlendOp alphaBlendOp = RHIBlendOp::Add;
  RHIColorComponentFlag colorWriteMask = {};
};

struct RHIPipelineLayoutCreateInfo {
  uint32_t setLayoutCount = {};
  const RHIDescriptorSetLayout* setLayouts = {};
  uint32_t pushConstantRangeCount = {};
  const RHIPushConstantRange* pushConstantRanges = {};
};
struct RHIDescriptorSetLayout {};
struct RHIPushConstantRange {
  RHIShaderStageFlag stageFlags = {};
  uint32_t offset = {};
  uint32_t size = {};
};

struct RHIRenderPassCreateInfo {
  uint32_t attachmentCount = {};
  const RHIAttachmentDescription* attachments = {};
  uint32_t subpassCount = {};
  const RHISubpassDescription* subpasses = {};
  uint32_t dependencyCount = {};
  const RHISubpassDependency* dependencies = {};
};

struct RHIAttachmentDescription {
  RHIAttachmentDescriptionFlag flag = {};
  RHIFormat format = RHIFormat::Undefined;
  RHISampleCount samples = RHISampleCount::Count1;
  RHIAttachmentLoadOp loadOp = RHIAttachmentLoadOp::Load;
  RHIAttachmentStoreOp storeOp = RHIAttachmentStoreOp::Store;
  RHIAttachmentLoadOp stencilLoadOp = RHIAttachmentLoadOp::Load;
  RHIAttachmentStoreOp stencilStoreOp = RHIAttachmentStoreOp::Store;
  RHIImageLayout initialLayout = RHIImageLayout::Undefined;
  RHIImageLayout finalLayout = RHIImageLayout::Undefined;
};

struct RHISubpassDescription {
  RHISubpassDescriptionFlag flags = {};
  RHIPipelineBindPoint pipelineBindPoint = RHIPipelineBindPoint::Graphics;
  uint32_t inputAttachmentCount = {};
  const RHIAttachmentReference* inputAttachments = {};
  uint32_t colorAttachmentCount = {};
  const RHIAttachmentReference* colorAttachments = {};
  const RHIAttachmentReference* resolveAttachments = {};
  const RHIAttachmentReference* depthStencilAttachment = {};
  uint32_t preserveAttachmentCount = {};
  const uint32_t* preserveAttachments = {};
};

struct RHISubpassDependency {
  uint32_t srcSubpass = {};
  uint32_t dstSubpass = {};
  RHIPipelineStageFlag srcStageMask = {};
  RHIPipelineStageFlag dstStageMask = {};
  RHIAccessFlag srcAccessMask = {};
  RHIAccessFlag dstAccessMask = {};
  RHIDependencyFlag dependencyFlags = {};
};

struct RHIAttachmentReference {
  uint32_t attachment = {};
  RHIImageLayout layout = RHIImageLayout::Undefined;
};

#pragma endregion

struct RHISwapChainInfo {
  RHIExtend2D extent;
  RHIFormat imageFormat;
  RHIImageView* imageViews;
};

struct RHICommandBufferBeginInfo {
  RHICommandBufferUsageFlag flags;
  RHICommandBufferInheritanceInfo* inheritanceInfo;
};

struct RHICommandBufferInheritanceInfo {
  RHIRenderPass renderPass = {};
  uint32_t subpass = {};
  RHIFramebuffer framebuffer = {};
  RHIBool32 occlusionQueryEnable = {};
  RHIQueryControlFlag queryFlags = {};
  RHIQueryPipelineStatisticFlag pipelineStatistics = {};
};

struct RHIClearDepthStencilValue {
  float depth = {};
  uint32_t stencil = {};
};
union RHIClearColorValue {
  float float32[4];
  int32_t int32[4];
  uint32_t uint32[4];
};
union RHIClearValue {
  RHIClearColorValue color;
  RHIClearDepthStencilValue depthStencil;
};
struct RHIRenderPassBeginInfo {
  RHIRenderPass* renderPass;
  RHIFramebuffer* frameBuffer;
  RHIRect2D renderArea;
  uint32_t clearValueCount;
  const RHIClearValue* clearValue;
};

struct RHIFramebufferCreateInfo {
  RHIRenderPass* renderPass;
  uint32_t attachmentCount;
  RHIImageView* const* attachments;
  uint32_t width;
  uint32_t height;
  uint32_t layers;
};

struct RHIBufferCreateInfo {
  RHIDeviceSize size = {};
  RHIBufferUsageFlag usage = {};
  RHISharingMode sharingMode = RHISharingMode::Exclusive;
  uint32_t queueFamilyIndexCount = {};
  const uint32_t* queueFamilyIndices = {};
};

}  // namespace Sparrow

#endif