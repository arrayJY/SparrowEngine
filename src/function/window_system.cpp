//
// Created by arrayJY on 2022/12/6.
//

#include "window_system.h"
#include <GLFW/glfw3.h>
#include <stdexcept>

namespace Sparrow {

WindowSystem::~WindowSystem() {
  glfwDestroyWindow(window);
  glfwTerminate();
}
void WindowSystem::initialize(const WindowSystemInitInfo& initInfo) {
  if (!glfwInit()) {
    throw std::runtime_error("GLFW init failed.");
  }

  width = initInfo.width, height = initInfo.height;
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window = glfwCreateWindow(width, height, "Sparrow Engine", nullptr, nullptr);
  if (!window) {
    throw std::runtime_error("GLFW create window failed.");
  }
}
void WindowSystem::pollEvents() const {
  glfwPollEvents();
}
bool WindowSystem::shouldClose() const {
  return glfwWindowShouldClose(window);
}
GLFWwindow* WindowSystem::getWindow() const {
  return window;
}
/*
auto WindowSystem::getWindowSize() const {
  return std::make_tuple(width, height);
}
 */
}  // namespace Sparrow