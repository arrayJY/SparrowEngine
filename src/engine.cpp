//
// Created by arrayJY on 2022/12/08.
//

#include "engine.h"
#include "function/render_system.h"
#include "function/window_system.h"
#include "global_context.h"

namespace Sparrow {

void Engine::startEngine() {
  gContext.initialize();
  mainLoop();
}

void Engine::tick(float deltaTime) {
  logicalTick(deltaTime);
  renderTick(deltaTime);
}

void Engine::shutdown() {}

void Engine::logicalTick(float deltaTime) {}

void Engine::renderTick(float deltaTime) {
  gContext.renderSystem->tick(deltaTime);
}

float Engine::calcOneFrameDeltaTime() const {
  return 0.0;  // TODO
}

void Engine::mainLoop() {
  while (!gContext.windowSystem->shouldClose()) {
    tick(calcOneFrameDeltaTime());
    gContext.windowSystem->pollEvents();
  }
}

}  // namespace Sparrow