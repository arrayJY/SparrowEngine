//
// Created by arrayJY on 2022/12/14.
//

#ifndef SPARROWENGINE_RHI_STRUCT_H
#define SPARROWENGINE_RHI_STRUCT_H

#include <array>
#include <cstdint>

// #include <vulkan/vulkan.hpp>

namespace Sparrow {

using RHIBool32 = uint32_t;
static constexpr RHIBool32 RHIFalse = 0U;
static constexpr RHIBool32 RHITrue = 1U;

enum class RHIShaderStageFlag : int {
  Vertex = 0x00000001,
  TessellationControl = 0x00000002,
  TessellationEvaluation = 0x00000004,
  Geometry = 0x00000008,
  Fragment = 0x00000010,
  Compute = 0x00000020,
  AllGraphics = 0x0000001F,
  All = 0x7FFFFFFF,
  RaygenKHR = 0x00000100,
  AnyHitKHR = 0x00000200,
  ClosestHitKHR = 0x00000400,
  MissKHR = 0x00000800,
  IntersectionKHR = 0x00001000,
  CallableKHR = 0x00002000,
  TaskNV = 0x00000040,
  MeshNV = 0x00000080,
  SubpassShadingHUAWEI = 0x00004000,
  AnyHitNV = RHIShaderStageFlag::AnyHitKHR,
  CallableNV = RHIShaderStageFlag::CallableKHR,
  ClosestHitNV = RHIShaderStageFlag::ClosestHitKHR,
  IntersectionNV = RHIShaderStageFlag::IntersectionKHR,
  MissNV = RHIShaderStageFlag::MissKHR,
  RaygenNV = RHIShaderStageFlag::RaygenKHR,
};

enum class RHIVertexInputRate : int {
  Vertex = 0,
  Instance = 1,
};

enum class RHIFormat : int {
  Undefined = 0,
  // TODO
};

enum class RHIDynamicState {
  // TODO

};
enum class RHIPrimitiveTopology {
  // TODO

};

enum class RHIPolygonMode {
  Fill = 0,
  Line = 1,
  Point = 2,
  FillRectangleNV = 1000153000,
};

enum class RHIFrontFace {
  CounterClockwise = 0,
  Clockwise = 1,
};
enum class RHICullMode {
  None = 0,
  Front = 1,
  Back = 2,
  FrontAndBack = 3,
};

enum class RHISampleCount : int {
  Count1 = 0x00000001,
  Count2 = 0x00000002,
  Count4 = 0x00000004,
  Count8 = 0x00000008,
  Count16 = 0x00000010,
  Count32 = 0x00000020,
  Count64 = 0x00000040,
};
using RHISampleMask = uint32_t;

enum class RHICompareOp : int {
  Never = 0,
  Less = 1,
  Equal = 2,
  LessOrEqual = 3,
  Greater = 4,
  NotEqual = 5,
  GreaterOrEqual = 6,
  Always = 7
};
enum class RHIStencilOp {
  Keep = 0,
  Zero = 1,
  Replace = 2,
  IncrementAndClamp = 3,
  DecrementAndClamp = 4,
  Invert = 5,
  IncrementAndWrap = 6,
  DecrementAndWrap = 7
};

enum class RHILogicOp {
  Clear = 0,
  And = 1,
  AndReverse = 2,
  Copy = 3,
  AndInverted = 4,
  NoOp = 5,
  Xor = 6,
  Or = 7,
  Nor = 8,
  Equivalent = 9,
  Invert = 10,
  OrReverse = 11,
  CopyInverted = 12,
  OrInverted = 13,
  Nand = 14,
  Set = 15,
};

enum class RHIAttachmentLoadOp {
  Load = 0,
  Clear = 1,
  DontCare = 2,
  NoneEXT = 1000400000,
};

enum class RHIAttachmentStoreOp {
  Store = 0,
  DontCare = 1,
  None = 1000301000,
  NoneEXT = RHIAttachmentStoreOp::None,
  NoneKHR = RHIAttachmentStoreOp::None,
  NoneQCOM = RHIAttachmentStoreOp::None,
};

enum class RHIImageLayout {
  Undefined = 0,
  // TODO
};

enum class RHIPipelineBindPoint {
  Graphics = 0,
  Compute = 1,
  RayTracingKHR = 1000165000,
  SubpassShadingHUAWEI = 1000369003,
  RayTracingNV = RHIPipelineBindPoint::RayTracingKHR
};

enum class RHIAccessFlag : int {
  // TODO
};

enum class RHIPipelineStageFlag : int {
  // TODO
};

enum class RHIDependencyFlag : int {
  ByRegion = 0x00000001,
  DeviceGroup = 0x00000004,
  ViewLocal = 0x00000002,
  FeedbackLoopEXT = 0x00000008,
  DeviceGroupKHR = RHIDependencyFlag::DeviceGroup,
  ViewLocalKHR = RHIDependencyFlag::ViewLocal,
};

template <typename T, std::size_t Size>
using RHIArray = std::array<T, Size>;

#pragma region Classes

class RHIPipeline {};
class RHIShader {};
class RHIPipelineLayout {};
class RHIRenderPass {};

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

struct RHIVertexBindingDescriptions;
struct RHIVertexAttributeDescriptions;

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
  const RHIVertexBindingDescriptions* vertexBindingDescriptions;
  uint32_t vertexAttributeDescriptionCount;
  const RHIVertexAttributeDescriptions* vertexAttributeDescriptions;
};
struct RHIVertexBindingDescriptions {
  uint32_t binding = {};
  uint32_t stride = {};
  RHIVertexInputRate inputRate = RHIVertexInputRate::Vertex;
};
struct RHIVertexAttributeDescriptions {
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

}  // namespace Sparrow

#endif