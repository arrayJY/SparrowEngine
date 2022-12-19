#ifndef SPARROWENGINE_RHI_H
#define SPARROWENGINE_RHI_H

#include <memory>
#include "rhi_struct.h"

namespace Sparrow {
class WindowSystem;
struct RHIInitInfo {
  std::shared_ptr<WindowSystem> windowSystem;
};

class RHI {
 public:
  virtual void initialize(const RHIInitInfo& initInfo) = 0;
  // creation
  virtual void createSwapChain() = 0;
  virtual void createSwapChainImageView() = 0;
  virtual void createFramebufferImageAndView() = 0;
  virtual bool createGraphicsPipeline(
      const RHIGraphicsPipelineCreateInfo& createInfo) = 0;
  virtual std::unique_ptr<RHIRenderPass> createRenderPass(const RHIRenderPassCreateInfo& createInfo) = 0;
  virtual std::unique_ptr<RHIPipelineLayout> createPipelineLayout(const RHIPipelineLayoutCreateInfo& createInfo) = 0;
 
  virtual ~RHI() = 0;
};

inline RHI::~RHI() = default;

};  // namespace Sparrow

#endif  // SPARROWENGINE_RHI_H