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
  virtual std::unique_ptr<RHIShader> createShaderModule(std::span<uint32_t> shader_code) = 0;
  virtual bool createGraphicsPipeline(
      const RHIGraphicsPipelineCreateInfo& createInfo) = 0;
  virtual std::unique_ptr<RHIRenderPass> createRenderPass(
      const RHIRenderPassCreateInfo& createInfo) = 0;
  virtual std::unique_ptr<RHIPipelineLayout> createPipelineLayout(
      const RHIPipelineLayoutCreateInfo& createInfo) = 0;
#pragma endregion

#pragma region Command
  virtual void submitRendering() = 0;
#pragma endregion

  virtual ~RHI() = 0;
};

inline RHI::~RHI() = default;

};  // namespace Sparrow

#endif  // SPARROWENGINE_RHI_H