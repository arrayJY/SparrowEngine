#ifndef SPARROWENGINE_RHI_H
#define SPARROWENGINE_RHI_H

#include <memory>
#include <span>
#include <vector>
#include "rhi_struct.h"

namespace Sparrow {
class WindowSystem;
struct RHIInitInfo {
  std::shared_ptr<WindowSystem> windowSystem;
};

class RHI {
 public:
  virtual void initialize(const RHIInitInfo& initInfo) = 0;

  /*** Creation ***/
  virtual void createSwapChain() = 0;
  virtual void recreateSwapChain() = 0;
  virtual void createSwapChainImageView() = 0;
  virtual void createFramebufferImageAndView() = 0;
  virtual std::unique_ptr<RHIFramebuffer> createFramebuffer(
      RHIFramebufferCreateInfo& createInfo) = 0;
  virtual std::unique_ptr<RHIShader> createShaderModule(
      std::span<char> shader_code) = 0;
  virtual std::unique_ptr<RHIPipeline> createGraphicsPipeline(
      const RHIGraphicsPipelineCreateInfo& createInfo) = 0;
  virtual std::unique_ptr<RHIRenderPass> createRenderPass(
      const RHIRenderPassCreateInfo& createInfo) = 0;
  virtual std::unique_ptr<RHIPipelineLayout> createPipelineLayout(
      const RHIPipelineLayoutCreateInfo& createInfo) = 0;
  virtual std::tuple<std::unique_ptr<RHIBuffer>,
                     std::unique_ptr<RHIDeviceMemory>>
  createBuffer(const RHIBufferCreateInfo& createInfo,
               RHIMemoryPropertyFlag properties) = 0;
  virtual std::tuple<std::unique_ptr<RHIImage>,
                     std::unique_ptr<RHIDeviceMemory>>
  createImage(const RHIImageCreateInfo& createInfo) = 0;
  virtual std::tuple<std::unique_ptr<RHIImage>,
                     std::unique_ptr<RHIImageView>,
                     std::unique_ptr<RHIDeviceMemory>>
  createImageAndCopyData(const RHIImageCreateInfo& createInfo,
                         void* data,
                         size_t dataSize) = 0;
  virtual std::unique_ptr<RHISampler> createSampler(
      const RHISamplerCreateInfo& createInfo) = 0;
  virtual std::unique_ptr<RHIDescriptorSetLayout> createDescriptorSetLayout(
      RHIDescriptorSetLayoutCreateInfo& createInfo) = 0;
  virtual std::vector<std::unique_ptr<RHIDescriptorSet>> allocateDescriptorSets(
      const RHIDescriptorSetAllocateInfo& allocateInfo) = 0;

  /*** Update ***/
  virtual void updateDescriptorSets(
      std::span<RHIWriteDescriptorSet> writeDescritorSets) = 0;

  /*** Query ***/
  virtual uint8_t getMaxFramesInFlight() = 0;
  virtual uint8_t getCurrentFrameIndex() = 0;
  virtual uint32_t getCurrentSwapChainImageIndex() = 0;
  virtual RHISwapChainInfo getSwapChainInfo() = 0;
  virtual RHIImageView* getSwapChainImageView(size_t index) = 0;
  virtual RHIDepthImageInfo getDepthImageInfo() = 0;
  virtual RHICommandBuffer* getCurrentCommandBuffer() = 0;
  virtual std::span<RHICommandBuffer> getCommandBuffers() = 0;

  /*** Destory ***/
  virtual void destoryBuffer(RHIBuffer* buffer) = 0;
  virtual void destoryDescriptorSetLayout(
      RHIDescriptorSetLayout* descriptorSetLayout) = 0;

  /*** Command ***/
  virtual bool beginCommandBuffer(
      RHICommandBuffer* commandBuffer,
      RHICommandBufferBeginInfo* commandBufferBeginInfo) = 0;
  virtual bool endCommandBuffer(RHICommandBuffer* commandBuffer) = 0;
  virtual std::unique_ptr<RHICommandBuffer> beginOneTimeCommandBuffer() = 0;
  virtual bool endOneTimeCommandBuffer(RHICommandBuffer* commandBuffer) = 0;
  virtual void beforePass() = 0;
  virtual void waitIdle() = 0;
  virtual void submitRendering() = 0;

  virtual void cmdBeginRenderPass(RHICommandBuffer* commandBuffer,
                                  RHIRenderPassBeginInfo* beginInfo,
                                  RHISubpassContents contents) = 0;
  virtual void cmdEndRenderPass(RHICommandBuffer* commandBuffer) = 0;
  virtual void cmdBindPipeline(RHICommandBuffer* commandBuffer,
                               RHIPipelineBindPoint bindPoint,
                               RHIPipeline* pipeline) = 0;
  virtual void cmdBindVertexBuffers(RHICommandBuffer* commandBuffer,
                                    uint32_t firstBinding,
                                    uint32_t bindingCount,
                                    RHIBuffer* const* buffers,
                                    const RHIDeviceSize* offsets) = 0;
  virtual void cmdBindIndexBuffer(RHICommandBuffer* commandBuffer,
                                  RHIBuffer* buffer,
                                  RHIDeviceSize offset,
                                  RHIIndexType indexType) = 0;
  virtual void cmdBindDescriptorSets(RHICommandBuffer* commandBuffer,
                                     RHIPipelineBindPoint pipelineBindPoint,
                                     const RHIPipelineLayout* layout,
                                     uint32_t firstSet,
                                     uint32_t descriptorSetCount,
                                     const RHIDescriptorSet* descriptorSets,
                                     uint32_t dynamicOffsetCount,
                                     const uint32_t* dynamicOffsets) = 0;
  virtual void cmdDraw(RHICommandBuffer* commandBuffer,
                       uint32_t vertexCount,
                       uint32_t instanceCount,
                       uint32_t firstVertex,
                       uint32_t firstInstance) = 0;
  virtual void cmdDrawIndexed(RHICommandBuffer* commandBuffer,
                              uint32_t indexCount,
                              uint32_t instanceCount,
                              uint32_t firstIndex,
                              int32_t vertexOffset,
                              uint32_t firstInstance) = 0;
  virtual void cmdSetViewport(RHICommandBuffer* commandBuffer,
                              uint32_t firstViewport,
                              uint32_t viewportCount,
                              const RHIViewport* pViewports) = 0;
  virtual void cmdSetScissor(RHICommandBuffer* commandBuffer,
                             uint32_t firstScissor,
                             uint32_t scissorCount,
                             const RHIRect2D* pScissors) = 0;
  virtual void cmdCopyBuffer(RHICommandBuffer* commandBuffer,
                             RHIBuffer* srcBuffer,
                             RHIBuffer* dstBuffer,
                             std::span<RHIBufferCopy> copyRegions) = 0;
  /*** Memory ***/
  virtual void* mapMemory(RHIDeviceMemory* deviceMemory,
                          RHIDeviceSize offset,
                          RHIDeviceSize size) = 0;
  virtual void unmapMemory(RHIDeviceMemory* deviceMemory) = 0;
  virtual void freeMemory(RHIDeviceMemory* deviceMemory) = 0;

  virtual ~RHI() = 0;
};

inline RHI::~RHI() = default;

};  // namespace Sparrow

#endif  // SPARROWENGINE_RHI_H