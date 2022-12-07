#ifndef SPARROWENGINE_RHI_H
#define SPARROWENGINE_RHI_H

#include <memory>

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

  virtual ~RHI() = 0;
};

inline RHI::~RHI() = default;

};  // namespace Sparrow

#endif  // SPARROWENGINE_RHI_H