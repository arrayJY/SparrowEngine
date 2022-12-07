//
// Created by arrayJY on 2022/12/08.
//

#include "global_context.h"
#include "function/render_system.h"
#include "function/window_system.h"

namespace Sparrow {
void GlobalContext::initialize() {
  windowSystem = std::make_shared<WindowSystem>();
  windowSystem->initialize({.width = 800, .height = 600});

  renderSystem = std::make_shared<RenderSystem>();
  renderSystem->initialize({.windowSystem = windowSystem});
}

GlobalContext gContext;

}  // namespace Sparrow