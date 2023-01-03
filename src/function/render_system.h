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
             std::vector<std::unique_ptr<RHIDeviceMemory>>>
  createUniformBuffers();
};

}  // namespace Sparrow

#endif