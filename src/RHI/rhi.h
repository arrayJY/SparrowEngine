#ifndef SPARROWENGINE_RHI_H
#define SPARROWENGINE_RHI_H

namespace Sparrow {
    class RHI {
    public:
        virtual void initialize() = 0;

        virtual ~RHI() = 0;
    };

    inline RHI::~RHI() = default;
};

#endif // SPARROWENGINE_RHI_H