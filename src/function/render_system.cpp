//
// Created by arrayJY on 2022/12/6.
//

#include "render_system.h"
#include "RHI/vulkan/vulkan_rhi.h"
#include "function/window_system.h"

namespace Sparrow {
void RenderSystem::initialize(const RenderSystemInitInfo& initInfo) {
  const auto rhiInitInfo = RHIInitInfo{.windowSystem = initInfo.windowSystem};
  rhi = std::make_shared<VulkanRHI>();
  rhi->initialize(rhiInitInfo);
}
void RenderSystem::tick(float deltaTime) {}

}  // namespace Sparrow