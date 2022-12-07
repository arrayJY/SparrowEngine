//
// Created by arrayJY on 2022/12/08.
//

#include "engine.h"
#include "global_context.h"

namespace Sparrow {

void Engine::startEngine() {
  gContext.initialize();
}

void Engine::tick(float deltaTime) {}

void Engine::shutdown() {}

void Engine::logicalTick(float deltaTime) {}

void Engine::renderTick(float deltaTime) {}

}  // namespace Sparrow