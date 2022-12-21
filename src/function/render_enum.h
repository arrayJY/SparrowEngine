//
// Created by arrayJY on 2022/12/21.
//

#ifndef SPARROWENGINE_RENDER_ENUM_H
#define SPARROWENGINE_RENDER_ENUM_H
#include <cstdint>

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

enum class RHICommandBufferUsageFlag : int {
  OneTimeSubmit = 0x00000001,
  RenderPassContinue = 0x00000002,
  SimultaneousUse = 0x00000004,
};
enum class RHIQueryControlFlag : int { Precise = 0x00000001 };
enum class RHIQueryPipelineStatisticFlag : int {
  InputAssemblyVertices = 0x00000001,
  InputAssemblyPrimitives = 0x00000002,
  VertexShaderInvocations = 0x00000004,
  GeometryShaderInvocations = 0x00000008,
  GeometryShaderPrimitives = 0x00000010,
  ClippingInvocations = 0x00000020,
  ClippingPrimitives = 0x00000040,
  FragmentShaderInvocations = 0x00000080,
  TessellationControlShaderPatches = 0x00000100,
  TessellationEvaluationShaderInvocations = 0x00000200,
  ComputeShaderInvocations = 0x00000400,
  TaskShaderInvocationsEXT = 0x00000800,
  MeshShaderInvocationsEXT = 0x00001000,
};

enum class RHISubpassContents {
  Inline = 0,
  SecondaryCommandBuffers = 1,
};

#endif