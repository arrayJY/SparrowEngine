//
// Created by arrayJY on 2022/12/7.
//

#ifndef SPARROWENGINE_GLOBAL_CONTEXT_H
#define SPARROWENGINE_GLOBAL_CONTEXT_H

#include <memory>

namespace Sparrow {
class RenderSystem;
class WindowSystem;

class GlobalContext {
 public:
  std::shared_ptr<RenderSystem> renderSystem = nullptr;
  std::shared_ptr<WindowSystem> windowSystem = nullptr;

 public:
  void initialize();
};

extern GlobalContext gContext;

}  // namespace Sparrow

#endif