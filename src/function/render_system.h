//
// Created by arrayJY on 2022/12/6.
//
#ifndef SPARROWENGINE_RENDER_SYSTEM_H
#define SPARROWENGINE_RENDER_SYSTEM_H

#include <memory>
#include <vector>
#include <string>

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
  static std::vector<uint32_t> readFile(const std::string& filename);
  std::shared_ptr<RHI> rhi;
};

}  // namespace Sparrow

#endif