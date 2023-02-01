//
// Created by arrayJY on 2022/12/21.
//

#ifndef SPARROWENGINE_RENDER_ENUM_H
#define SPARROWENGINE_RENDER_ENUM_H
#include <cstdint>
#include <type_traits>

using RHIFlag = int;

namespace Sparrow {

enum class RHIShaderStageFlag : RHIFlag {
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

enum class RHIVertexInputRate : RHIFlag {
  Vertex = 0,
  Instance = 1,
};

enum class RHIFormat : RHIFlag {
  Undefined = 0,
  R4G4UnormPack8 = 1,
  R4G4B4A4UnormPack16 = 2,
  B4G4R4A4UnormPack16 = 3,
  R5G6B5UnormPack16 = 4,
  B5G6R5UnormPack16 = 5,
  R5G5B5A1UnormPack16 = 6,
  B5G5R5A1UnormPack16 = 7,
  A1R5G5B5UnormPack16 = 8,
  R8Unorm = 9,
  R8Snorm = 10,
  R8Uscaled = 11,
  R8Sscaled = 12,
  R8Uint = 13,
  R8Sint = 14,
  R8Srgb = 15,
  R8G8Unorm = 16,
  R8G8Snorm = 17,
  R8G8Uscaled = 18,
  R8G8Sscaled = 19,
  R8G8Uint = 20,
  R8G8Sint = 21,
  R8G8Srgb = 22,
  R8G8B8Unorm = 23,
  R8G8B8Snorm = 24,
  R8G8B8Uscaled = 25,
  R8G8B8Sscaled = 26,
  R8G8B8Uint = 27,
  R8G8B8Sint = 28,
  R8G8B8Srgb = 29,
  B8G8R8Unorm = 30,
  B8G8R8Snorm = 31,
  B8G8R8Uscaled = 32,
  B8G8R8Sscaled = 33,
  B8G8R8Uint = 34,
  B8G8R8Sint = 35,
  B8G8R8Srgb = 36,
  R8G8B8A8Unorm = 37,
  R8G8B8A8Snorm = 38,
  R8G8B8A8Uscaled = 39,
  R8G8B8A8Sscaled = 40,
  R8G8B8A8Uint = 41,
  R8G8B8A8Sint = 42,
  R8G8B8A8Srgb = 43,
  B8G8R8A8Unorm = 44,
  B8G8R8A8Snorm = 45,
  B8G8R8A8Uscaled = 46,
  B8G8R8A8Sscaled = 47,
  B8G8R8A8Uint = 48,
  B8G8R8A8Sint = 49,
  B8G8R8A8Srgb = 50,
  A8B8G8R8UnormPack32 = 51,
  A8B8G8R8SnormPack32 = 52,
  A8B8G8R8UscaledPack32 = 53,
  A8B8G8R8SscaledPack32 = 54,
  A8B8G8R8UintPack32 = 55,
  A8B8G8R8SintPack32 = 56,
  A8B8G8R8SrgbPack32 = 57,
  A2R10G10B10UnormPack32 = 58,
  A2R10G10B10SnormPack32 = 59,
  A2R10G10B10UscaledPack32 = 60,
  A2R10G10B10SscaledPack32 = 61,
  A2R10G10B10UintPack32 = 62,
  A2R10G10B10SintPack32 = 63,
  A2B10G10R10UnormPack32 = 64,
  A2B10G10R10SnormPack32 = 65,
  A2B10G10R10UscaledPack32 = 66,
  A2B10G10R10SscaledPack32 = 67,
  A2B10G10R10UintPack32 = 68,
  A2B10G10R10SintPack32 = 69,
  R16Unorm = 70,
  R16Snorm = 71,
  R16Uscaled = 72,
  R16Sscaled = 73,
  R16Uint = 74,
  R16Sint = 75,
  R16Sfloat = 76,
  R16G16Unorm = 77,
  R16G16Snorm = 78,
  R16G16Uscaled = 79,
  R16G16Sscaled = 80,
  R16G16Uint = 81,
  R16G16Sint = 82,
  R16G16Sfloat = 83,
  R16G16B16Unorm = 84,
  R16G16B16Snorm = 85,
  R16G16B16Uscaled = 86,
  R16G16B16Sscaled = 87,
  R16G16B16Uint = 88,
  R16G16B16Sint = 89,
  R16G16B16Sfloat = 90,
  R16G16B16A16Unorm = 91,
  R16G16B16A16Snorm = 92,
  R16G16B16A16Uscaled = 93,
  R16G16B16A16Sscaled = 94,
  R16G16B16A16Uint = 95,
  R16G16B16A16Sint = 96,
  R16G16B16A16Sfloat = 97,
  R32Uint = 98,
  R32Sint = 99,
  R32Sfloat = 100,
  R32G32Uint = 101,
  R32G32Sint = 102,
  R32G32Sfloat = 103,
  R32G32B32Uint = 104,
  R32G32B32Sint = 105,
  R32G32B32Sfloat = 106,
  R32G32B32A32Uint = 107,
  R32G32B32A32Sint = 108,
  R32G32B32A32Sfloat = 109,
  R64Uint = 110,
  R64Sint = 111,
  R64Sfloat = 112,
  R64G64Uint = 113,
  R64G64Sint = 114,
  R64G64Sfloat = 115,
  R64G64B64Uint = 116,
  R64G64B64Sint = 117,
  R64G64B64Sfloat = 118,
  R64G64B64A64Uint = 119,
  R64G64B64A64Sint = 120,
  R64G64B64A64Sfloat = 121,
  B10G11R11UfloatPack32 = 122,
  E5B9G9R9UfloatPack32 = 123,
  D16Unorm = 124,
  X8D24UnormPack32 = 125,
  D32Sfloat = 126,
  S8Uint = 127,
  D16UnormS8Uint = 128,
  D24UnormS8Uint = 129,
  D32SfloatS8Uint = 130,
  Bc1RgbUnormBlock = 131,
  Bc1RgbSrgbBlock = 132,
  Bc1RgbaUnormBlock = 133,
  Bc1RgbaSrgbBlock = 134,
  Bc2UnormBlock = 135,
  Bc2SrgbBlock = 136,
  Bc3UnormBlock = 137,
  Bc3SrgbBlock = 138,
  Bc4UnormBlock = 139,
  Bc4SnormBlock = 140,
  Bc5UnormBlock = 141,
  Bc5SnormBlock = 142,
  Bc6HUfloatBlock = 143,
  Bc6HSfloatBlock = 144,
  Bc7UnormBlock = 145,
  Bc7SrgbBlock = 146,
  Etc2R8G8B8UnormBlock = 147,
  Etc2R8G8B8SrgbBlock = 148,
  Etc2R8G8B8A1UnormBlock = 149,
  Etc2R8G8B8A1SrgbBlock = 150,
  Etc2R8G8B8A8UnormBlock = 151,
  Etc2R8G8B8A8SrgbBlock = 152,
  EacR11UnormBlock = 153,
  EacR11SnormBlock = 154,
  EacR11G11UnormBlock = 155,
  EacR11G11SnormBlock = 156,
  Astc4x4UnormBlock = 157,
  Astc4x4SrgbBlock = 158,
  Astc5x4UnormBlock = 159,
  Astc5x4SrgbBlock = 160,
  Astc5x5UnormBlock = 161,
  Astc5x5SrgbBlock = 162,
  Astc6x5UnormBlock = 163,
  Astc6x5SrgbBlock = 164,
  Astc6x6UnormBlock = 165,
  Astc6x6SrgbBlock = 166,
  Astc8x5UnormBlock = 167,
  Astc8x5SrgbBlock = 168,
  Astc8x6UnormBlock = 169,
  Astc8x6SrgbBlock = 170,
  Astc8x8UnormBlock = 171,
  Astc8x8SrgbBlock = 172,
  Astc10x5UnormBlock = 173,
  Astc10x5SrgbBlock = 174,
  Astc10x6UnormBlock = 175,
  Astc10x6SrgbBlock = 176,
  Astc10x8UnormBlock = 177,
  Astc10x8SrgbBlock = 178,
  Astc10x10UnormBlock = 179,
  Astc10x10SrgbBlock = 180,
  Astc12x10UnormBlock = 181,
  Astc12x10SrgbBlock = 182,
  Astc12x12UnormBlock = 183,
  Astc12x12SrgbBlock = 184,
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

enum class RHIAttachmentDescriptionFlag : RHIFlag { MayAlias = 0x00000001 };
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
enum class RHIColorComponentFlag : RHIFlag {
  R = 0x00000001,
  G = 0x00000002,
  B = 0x00000004,
  A = 0x00000008,
  AllBits = RHIColorComponentFlag::R | RHIColorComponentFlag::G |
            RHIColorComponentFlag::B | RHIColorComponentFlag::A,
};

enum class RHISampleCount : RHIFlag {
  Count1 = 0x00000001,
  Count2 = 0x00000002,
  Count4 = 0x00000004,
  Count8 = 0x00000008,
  Count16 = 0x00000010,
  Count32 = 0x00000020,
  Count64 = 0x00000040,
};
using RHISampleMask = uint32_t;

enum class RHICompareOp : RHIFlag {
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
enum class RHISubpassDescriptionFlag : RHIFlag {
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

enum class RHIAccessFlag : RHIFlag {
  IndirectCommandRead = 0x00000001,
  IndexRead = 0x00000002,
  VertexAttributeRead = 0x00000004,
  UniformRead = 0x00000008,
  InputAttachmentRead = 0x00000010,
  ShaderRead = 0x00000020,
  ShaderWrite = 0x00000040,
  ColorAttachmentRead = 0x00000080,
  ColorAttachmentWrite = 0x00000100,
  DepthStencilAttachmentRead = 0x00000200,
  DepthStencilAttachmentWrite = 0x00000400,
  TransferRead = 0x00000800,
  TransferWrite = 0x00001000,
  HostRead = 0x00002000,
  HostWrite = 0x00004000,
  MemoryRead = 0x00008000,
  MemoryWrite = 0x00010000,
  None = 0,
  TransformFeedbackWriteEXT = 0x02000000,
  TransformFeedbackCounterReadEXT = 0x04000000,
  TransformFeedbackCounterWriteEXT = 0x08000000,
  ConditionalRenderingReadEXT = 0x00100000,
  ColorAttachmentReadNoncoherentEXT = 0x00080000,
  AccelerationStructureReadKHR = 0x00200000,
  AccelerationStructureWriteKHR = 0x00400000,
  FragmentDensityMapReadEXT = 0x01000000,
  FragmentShadingRateAttachmentReadKHR = 0x00800000,
  CommandPreprocessReadNV = 0x00020000,
  CommandPreprocessWriteNV = 0x00040000,
  AccelerationStructureReadNV = RHIAccessFlag::AccelerationStructureReadKHR,
  AccelerationStructureWriteNV = RHIAccessFlag::AccelerationStructureWriteKHR,
  NoneKHR = RHIAccessFlag::None,
  ShadingRateImageReadNV = RHIAccessFlag::FragmentShadingRateAttachmentReadKHR,
};

enum class RHIPipelineStageFlag : RHIFlag {
  TopOfPipe = 0x00000001,
  DrawIndirect = 0x00000002,
  VertexInput = 0x00000004,
  VertexShader = 0x00000008,
  TessellationControlShader = 0x00000010,
  TessellationEvaluationShader = 0x00000020,
  GeometryShader = 0x00000040,
  FragmentShader = 0x00000080,
  EarlyFragmentTests = 0x00000100,
  LateFragmentTests = 0x00000200,
  ColorAttachmentOutput = 0x00000400,
  ComputeShader = 0x00000800,
  Transfer = 0x00001000,
  BottomOfPipe = 0x00002000,
  Host = 0x00004000,
  AllGraphics = 0x00008000,
  AllCommands = 0x00010000,
  None = 0,
  TransformFeedbackEXT = 0x01000000,
  ConditionalRenderingEXT = 0x00040000,
  AccelerationStructureBuildKHR = 0x02000000,
  RayTracingShaderKHR = 0x00200000,
  FragmentDensityProcessEXT = 0x00800000,
  FragmentShadingRateAttachmentKHR = 0x00400000,
  CommandPreprocessNV = 0x00020000,
  TaskShaderEXT = 0x00080000,
  MeshShaderEXT = 0x00100000,
  AccelerationStructureBuildNV =
      RHIPipelineStageFlag::AccelerationStructureBuildKHR,
  MeshShaderNV = RHIPipelineStageFlag::MeshShaderEXT,
  NoneKHR = RHIPipelineStageFlag::None,
  RayTracingShaderNV = RHIPipelineStageFlag::RayTracingShaderKHR,
  ShadingRateImageNV = RHIPipelineStageFlag::FragmentShadingRateAttachmentKHR,
  TaskShaderNV = RHIPipelineStageFlag::TaskShaderEXT,
};

enum class RHIDependencyFlag : RHIFlag {
  ByRegion = 0x00000001,
  DeviceGroup = 0x00000004,
  ViewLocal = 0x00000002,
  FeedbackLoopEXT = 0x00000008,
  DeviceGroupKHR = RHIDependencyFlag::DeviceGroup,
  ViewLocalKHR = RHIDependencyFlag::ViewLocal,
};

enum class RHICommandBufferUsageFlag : RHIFlag {
  OneTimeSubmit = 0x00000001,
  RenderPassContinue = 0x00000002,
  SimultaneousUse = 0x00000004,
};
enum class RHIQueryControlFlag : RHIFlag { Precise = 0x00000001 };
enum class RHIQueryPipelineStatisticFlag : RHIFlag {
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

enum class RHIBufferUsageFlag : RHIFlag {
  TransferSrc = 0x00000001,
  TransferDst = 0x00000002,
  UniformTexelBuffer = 0x00000004,
  StorageTexelBuffer = 0x00000008,
  UniformBuffer = 0x00000010,
  StorageBuffer = 0x00000020,
  IndexBuffer = 0x00000040,
  VertexBuffer = 0x00000080,
  IndirectBuffer = 0x00000100,
  ShaderDeviceAddress = 0x00020000,
  TransformFeedbackBufferEXT = 0x00000800,
  TransformFeedbackCounterBufferEXT = 0x00001000,
  ConditionalRenderingEXT = 0x00000200,
  AccelerationStructureBuildInputReadOnlyKHR = 0x00080000,
  AccelerationStructureStorageKHR = 0x00100000,
  ShaderBindingTableKHR = 0x00000400,
  RayTracingNV = RHIBufferUsageFlag::ShaderBindingTableKHR,
  ShaderDeviceAddressEXT = RHIBufferUsageFlag::ShaderDeviceAddress,
  ShaderDeviceAddressKHR = RHIBufferUsageFlag::ShaderDeviceAddress,
};

enum class RHISharingMode { Exclusive = 0, Concurrent = 1 };

enum class RHIMemoryPropertyFlag : RHIFlag {
  DeviceLocal = 0x00000001,
  HostVisible = 0x00000002,
  HostCoherent = 0x00000004,
  HostCached = 0x00000008,
  LazilyAllocated = 0x00000010,
  Protected = 0x00000020,
  DeviceCoherentAMD = 0x00000040,
  DeviceUncachedAMD = 0x00000080,
  RdmaCapableNV = 0x00000100,
};

enum class RHIIndexType {
  Uint16 = 0,
  Uint32 = 1,
  NoneKHR = 1000165000,
  Uint8EXT = 1000265000,
  NoneNV = RHIIndexType::NoneKHR,
};

enum class RHIDescriptorType {
  Sampler = 0,
  CombinedImageSampler = 1,
  SampledImage = 2,
  StorageImage = 3,
  UniformTexelBuffer = 4,
  StorageTexelBuffer = 5,
  UniformBuffer = 6,
  StorageBuffer = 7,
  UniformBufferDynamic = 8,
  StorageBufferDynamic = 9,
  InputAttachment = 10,
  InlineUniformBlock = 1000138000,
  AccelerationStructureKHR = 1000150000,
  AccelerationStructureNV = 1000165000,
  SampleWeightImageQCOM = 1000440000,
  BlockMatchImageQCOM = 1000440001,
  MutableEXT = 1000351000,
  InlineUniformBlockEXT = RHIDescriptorType::InlineUniformBlock,
  MutableVALVE = RHIDescriptorType::MutableEXT
};

enum class RHIImageTiling {
  Optimal = 0,
  Linear = 1,
  DrmFormatModifierEXT = 1000158000,
};

enum class RHIImageUsageFlag {
  TransferSrc = 0x00000001,
  TransferDst = 0x00000002,
  Sampled = 0x00000004,
  Storage = 0x00000008,
  ColorAttachment = 0x00000010,
  DepthStencilAttachment = 0x00000020,
  TransientAttachment = 0x00000040,
  InputAttachment = 0x00000080,
  FragmentDensityMapEXT = 0x00000200,
  FragmentShadingRateAttachmentKHR = 0x00000100,
  AttachmentFeedbackLoopEXT = 0x00080000,
  InvocationMaskHUAWEI = 0x00040000,
  SampleWeightQCOM = 0x00100000,
  SampleBlockMatchQCOM = 0x00200000,
  ShadingRateImageNV = RHIImageUsageFlag::FragmentShadingRateAttachmentKHR,
};

enum class RHIImageCreateFlag {
  SparseBinding = 0x00000001,
  SparseResidency = 0x00000002,
  SparseAliased = 0x00000004,
  MutableFormat = 0x00000008,
  CubeCompatible = 0x00000010,
  Alias = 0x00000400,
  SplitInstanceBindRegions = 0x00000040,
  _2DArrayCompatible = 0x00000020,
  BlockTexelViewCompatible = 0x00000080,
  ExtendedUsage = 0x00000100,
  Protected = 0x00000800,
  Disjoint = 0x00000200,
  CornerSampledNV = 0x00002000,
  SampleLocationsCompatibleDepthEXT = 0x00001000,
  SubsampledEXT = 0x00004000,
  MultisampledRenderToSingleSampledEXT = 0x00040000,
  _2DViewCompatibleEXT = 0x00020000,
  FragmentDensityMapOffsetQCOM = 0x00008000,
  _2DArrayCompatibleKHR = RHIImageCreateFlag::_2DArrayCompatible,
  AliasKHR = RHIImageCreateFlag::Alias,
  BlockTexelViewCompatibleKHR = RHIImageCreateFlag::BlockTexelViewCompatible,
  DisjointKHR = RHIImageCreateFlag::Disjoint,
  ExtendedUsageKHR = RHIImageCreateFlag::ExtendedUsage,
  SplitInstanceBindRegionsKHR = RHIImageCreateFlag::SplitInstanceBindRegions,
};

enum class RHIBorderColor {
  FloatTransparentBlack = 0,
  IntTransparentBlack = 1,
  FloatOpaqueBlack = 2,
  IntOpaqueBlack = 3,
  FloatOpaqueWhite = 4,
  IntOpaqueWhite = 5,
  FloatCustomEXT = 1000287003,
  IntCustomEXT = 1000287004,
};

enum class RHISamplerAddressMode {
  Repeat = 0,
  MirroredRepeat = 1,
  ClampToEdge = 2,
  ClampToBorder = 3,
  MirrorClampToEdge = 4,
  MirrorClampToEdgeKHR = RHISamplerAddressMode::MirrorClampToEdge,
};

enum class RHISamplerMipmapMode {
  Nearest = 0,
  Linear = 1,
};

enum class RHIFilter {
  Nearest = 0,
  Linear = 1,
  CubicEXT = 1000015000,
  CubicIMG = RHIFilter::CubicEXT
};

template <typename EnumType>
struct RHIFlagEnum : public std::false_type {};

template <typename T>
concept IsRHIFlag = std::is_base_of_v<std::true_type, RHIFlagEnum<T>>;

template <typename EnumType>
  requires IsRHIFlag<EnumType>
constexpr EnumType operator&(EnumType l, EnumType r) {
  return static_cast<EnumType>(
      static_cast<std::underlying_type_t<EnumType>>(l) &
      static_cast<std::underlying_type_t<EnumType>>(r));
}
template <typename EnumType>
  requires IsRHIFlag<EnumType>
constexpr EnumType operator|(EnumType l, EnumType r) {
  return static_cast<EnumType>(
      static_cast<std::underlying_type_t<EnumType>>(l) |
      static_cast<std::underlying_type_t<EnumType>>(r));
}

#define DEF_RHI_FLAG_ENUM_TYPE(Type) \
  template <>                        \
  struct RHIFlagEnum<Type> : public std::true_type {};
DEF_RHI_FLAG_ENUM_TYPE(RHIShaderStageFlag);
DEF_RHI_FLAG_ENUM_TYPE(RHIAttachmentDescriptionFlag);
DEF_RHI_FLAG_ENUM_TYPE(RHIColorComponentFlag);
DEF_RHI_FLAG_ENUM_TYPE(RHISubpassDescriptionFlag);
DEF_RHI_FLAG_ENUM_TYPE(RHIAccessFlag);
DEF_RHI_FLAG_ENUM_TYPE(RHIPipelineStageFlag);
DEF_RHI_FLAG_ENUM_TYPE(RHIDependencyFlag);
DEF_RHI_FLAG_ENUM_TYPE(RHICommandBufferUsageFlag);
DEF_RHI_FLAG_ENUM_TYPE(RHIQueryControlFlag);
DEF_RHI_FLAG_ENUM_TYPE(RHIQueryPipelineStatisticFlag);
DEF_RHI_FLAG_ENUM_TYPE(RHIBufferUsageFlag);
DEF_RHI_FLAG_ENUM_TYPE(RHIMemoryPropertyFlag);
DEF_RHI_FLAG_ENUM_TYPE(RHIImageUsageFlag);
DEF_RHI_FLAG_ENUM_TYPE(RHIImageCreateFlag);
}  // namespace Sparrow
#endif