//
// Created by arrayJY on 2022/12/6.
//
#ifndef SPARROWENGINE_RENDER_SYSTEM_H
#define SPARROWENGINE_RENDER_SYSTEM_H

#include <memory>
#include <span>
#include <string>
#include <vector>
#include "RHI/rhi_struct.h"
#include "render_mesh.h"

namespace Sparrow {
class WindowSystem;
class RHI;

struct RenderSystemInitInfo {
  std::shared_ptr<WindowSystem> windowSystem;
};

class RenderSystem {
 public:
  RenderSystem() = default;
  void initialize(const RenderSystemInitInfo& initInfo);
  void tick(float deltaTime);

 private:
  static std::vector<char> readFile(const std::string& filename);
  std::shared_ptr<RHI> rhi;

  std::tuple<std::unique_ptr<RHIBuffer>, std::unique_ptr<RHIDeviceMemory>>
  createIndexBuffer(std::span<uint16_t> indices);

  std::tuple<std::unique_ptr<RHIBuffer>, std::unique_ptr<RHIDeviceMemory>>
  createVertexBuffer(std::span<struct Vertex> vertices);

  std::tuple<std::vector<std::unique_ptr<RHIBuffer>>,
             std::vector<std::unique_ptr<RHIDeviceMemory>>,
             std::vector<void*>>
  createUniformBuffers();
  std::vector<std::unique_ptr<RHIBuffer>> uniformBuffers;
  std::vector<std::unique_ptr<RHIDeviceMemory>> uniformBufferMemories;
  std::vector<void*> uniformBuffersMappedMemories;

  void updateUniformBuffer(void* mappedMemory);

  void recordCommandBuffer(RHICommandBuffer* commandBuffer);
  RHIViewport viewport;
  RHIRect2D scissor;
  std::vector<Vertex> vertices;
  std::vector<uint16_t> indices;

  std::unique_ptr<RHIShader> vertexShader, fragmentShader;
  std::unique_ptr<RHIDescriptorSetLayout> descriptorSetLayout;
  std::vector<std::unique_ptr<RHIDescriptorSet>> descriptorSets;
  std::unique_ptr<RHIRenderPass> renderPass;
  std::unique_ptr<RHIPipelineLayout> piplineLayout;
  std::vector<std::unique_ptr<RHIFramebuffer>> framebuffers;
  std::unique_ptr<RHIPipeline> graphicsPipeline;
  std::unique_ptr<RHIBuffer> indexBuffer, vertexBuffer;
  std::unique_ptr<RHIDeviceMemory> indexBufferMemory, vertexBufferMemory;
};

}  // namespace Sparrow

#endif