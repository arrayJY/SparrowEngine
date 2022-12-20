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
  Viewport = 0,
  Scissor = 1,
  LineWidth = 2,
  DepthBias = 3,
  BlendConstants = 4,
  DepthBounds = 5,
  StencilCompareMask = 6,
  StencilWriteMask = 7,
  StencilReference = 8,
  CullMode = 1000267000,
  FrontFace = 1000267001,
  PrimitiveTopology = 1000267002,
  ViewportWithCount = 1000267003,
  ScissorWithCount = 1000267004,
  VertexInputBindingStride = 1000267005,
  DepthTestEnable = 1000267006,
  DepthWriteEnable = 1000267007,
  DepthCompareOp = 1000267008,
  DepthBoundsTestEnable = 1000267009,
  StencilTestEnable = 1000267010,
  StencilOp = 1000267011,
  RasterizerDiscardEnable = 1000377001,
  DepthBiasEnable = 1000377002,
  PrimitiveRestartEnable = 1000377004,
  ViewportWScalingNV = 1000087000,
  DiscardRectangleEXT = 1000099000,
  SampleLocationsEXT = 1000143000,
  RayTracingPipelineStackSizeKHR = 1000347000,
  ViewportShadingRatePaletteNV = 1000164004,
  ViewportCoarseSampleOrderNV = 1000164006,
  ExclusiveScissorNV = 1000205001,
  // TODO
};

enum class RHIPrimitiveTopology {
  PointList = 0,
  LineList = 1,
  LineStrip = 2,
  TriangleList = 3,
  TriangleStrip = 4,
  TriangleFan = 5,
  LineListWithAdjacency = 6,
  LineStripWithAdjacency = 7,
  TriangleListWithAdjacency = 8,
  TriangleStripWithAdjacency = 9,
  PatchList = 10,
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

enum class RHIAttachmentDescriptionFlag : int { MayAlias = 0x00000001 };
enum class RHIBlendFactor {
  Zero = 0,
  One = 1,
  SrcColor = 2,
  OneMinusSrcColor = 3,
  DstColor = 4,
  OneMinusDstColor = 5,
  SrcAlpha = 6,
  OneMinusSrcAlpha = 7,
  DstAlpha = 8,
  OneMinusDstAlpha = 9,
  ConstantColor = 10,
  OneMinusConstantColor = 11,
  ConstantAlpha = 12,
  OneMinusConstantAlpha = 13,
  SrcAlphaSaturate = 14,
  Src1Color = 15,
  OneMinusSrc1Color = 16,
  Src1Alpha = 17,
  OneMinusSrc1Alpha = 18,
};
enum class RHIBlendOp {
  Add = 0,
  Subtract = 1,
  ReverseSubtract = 2,
  Min = 3,
  Max = 4,
  // TODO
};
enum class RHIColorComponentFlag : int {
  R = 0x00000001,
  G = 0x00000002,
  B = 0x00000004,
  A = 0x00000008,
  AllBits = RHIColorComponentFlag::R | RHIColorComponentFlag::G |
            RHIColorComponentFlag::B | RHIColorComponentFlag::A,
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
enum class RHISubpassDescriptionFlag : int {
  PerViewAttributesNVX = 0x00000001,
  PerViewPositionXOnlyNVX = 0x00000002,
  FragmentRegionQCOM = 0x00000004,
  ShaderResolveQCOM = 0x00000008,
  RasterizationOrderAttachmentColorAccessEXT = 0x00000010,
  RasterizationOrderAttachmentDepthAccessEXT = 0x00000020,
  RasterizationOrderAttachmentStencilAccessEXT = 0x00000040,
  EnableLegacyDitheringEXT = 0x00000080,
  RasterizationOrderAttachmentColorAccessARM =
      RHISubpassDescriptionFlag::RasterizationOrderAttachmentColorAccessEXT,
  RasterizationOrderAttachmentDepthAccessARM =
      RHISubpassDescriptionFlag::RasterizationOrderAttachmentDepthAccessEXT,
  RasterizationOrderAttachmentStencilAccessARM =
      RHISubpassDescriptionFlag::RasterizationOrderAttachmentStencilAccessEXT,
};

enum class RHIImageLayout {
  Undefined = 0,
  General = 1,
  ColorAttachmentOptimal = 2,
  DepthStencilAttachmentOptimal = 3,
  DepthStencilReadOnlyOptimal = 4,
  ShaderReadOnlyOptimal = 5,
  TransferSrcOptimal = 6,
  TransferDstOptimal = 7,
  Preinitialized = 8,
  DepthReadOnlyStencilAttachmentOptimal = 1000117000,
  DepthAttachmentStencilReadOnlyOptimal = 1000117001,
  DepthAttachmentOptimal = 1000241000,
  DepthReadOnlyOptimal = 1000241001,
  StencilAttachmentOptimal = 1000241002,
  StencilReadOnlyOptimal = 1000241003,
  ReadOnlyOptimal = 1000314000,
  AttachmentOptimal = 1000314001,
  PresentSrcKHR = 1000001002,
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
};

}  // namespace Sparrow

#endif