//
// Created by arrayJY on 2022/12/6.
//

#ifndef SPARROWENGINE_WINDOW_SYSTEM_H
#define SPARROWENGINE_WINDOW_SYSTEM_H

#include <tuple>

struct GLFWwindow;
namespace Sparrow {
struct WindowSystemInitInfo {
  int width = 0;
  int height = 0;
};
class WindowSystem {
 public:
  WindowSystem() = default;
  ~WindowSystem();
  void initialize(const WindowSystemInitInfo& initInfo);
  void pollEvents() const;
  [[nodiscard]] bool shouldClose() const;
  [[nodiscard]] GLFWwindow* getWindow() const;
  [[nodiscard]] auto getWindowSize() const {
    return std::make_tuple(width, height);
  }

 private:
  GLFWwindow* window = nullptr;
  int width = 0, height = 0;
};

}  // namespace Sparrow

#endif  // SPARROWENGINE_WINDOW_SYSTEM_H
