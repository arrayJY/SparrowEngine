#ifndef SPARROWENGINE_RHI_H
#define SPARROWENGINE_RHI_H

#include <memory>
#include <span>
#include "rhi_struct.h"

namespace Sparrow {
class WindowSystem;
struct RHIInitInfo {
  std::shared_ptr<WindowSystem> windowSystem;
};

class RHI {
 public:
  virtual void initialize(const RHIInitInfo& initInfo) = 0;
#pragma region Creation
  virtual void createSwapChain() = 0;
  virtual void createSwapChainImageView() = 0;
  virtual void createFramebufferImageAndView() = 0;
  virtual std::unique_ptr<RHIShader> createShaderModule(
      std::span<char> shader_code) = 0;
  virtual bool createGraphicsPipeline(
      const RHIGraphicsPipelineCreateInfo& createInfo) = 0;
  virtual std::unique_ptr<RHIRenderPass> createRenderPass(
      const RHIRenderPassCreateInfo& createInfo) = 0;
  virtual std::unique_ptr<RHIPipelineLayout> createPipelineLayout(
      const RHIPipelineLayoutCreateInfo& createInfo) = 0;
#pragma endregion

#pragma region Query
  virtual RHISwapChainInfo getSwapChainInfo() = 0;
#pragma endregion

#pragma region Command
  virtual bool beginCommandBuffer(
      RHICommandBuffer* commandBuffer,
      RHICommandBufferBeginInfo* commandBufferBeginInfo) = 0;
  virtual bool endCommandBuffer(RHICommandBuffer* commandBuffer) = 0;
  virtual void submitRendering() = 0;

  virtual void cmdBeginRenderPass(RHICommandBuffer* commandBuffer,
                                  RHIRenderPassBeginInfo* beginInfo,
                                  RHISubpassContents contents) = 0;
  virtual void cmdEndRenderPass(RHICommandBuffer* commandBuffer) = 0;
  virtual void cmdBindPipeline(RHICommandBuffer* commandBuffer,
                               RHIPipelineBindPoint bindPoint,
                               RHIPipeline* pipeline) = 0;
  virtual void cmdDraw(RHICommandBuffer* commandBuffer,
                       uint32_t vertexCount,
                       uint32_t instanceCount,
                       uint32_t firstVertex,
                       uint32_t firstInstance) = 0;
  virtual void cmdSetViewport(RHICommandBuffer* commandBuffer,
                              uint32_t firstViewport,
                              uint32_t viewportCount,
                              const RHIViewport* pViewports) = 0;
  virtual void cmdSetScissor(RHICommandBuffer* commandBuffer,
                             uint32_t firstScissor,
                             uint32_t scissorCount,
                             const RHIRect2D* pScissors) = 0;

#pragma endregion

  virtual ~RHI() = 0;
};

inline RHI::~RHI() = default;

};  // namespace Sparrow

#endif  // SPARROWENGINE_RHI_H