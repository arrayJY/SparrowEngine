#ifndef SPARROWENGINE_RHI_H
#define SPARROWENGINE_RHI_H

namespace Sparrow {
class RHI {
public:
    virtual void initialize() = 0;
    // creation
    virtual void createSwapChain() = 0;
    virtual void createSwapChainImageView() = 0;
    virtual void createFramebufferImageAndView() = 0;

    virtual ~RHI() = 0;
};

inline RHI::~RHI() = default;
};

#endif // SPARROWENGINE_RHI_H