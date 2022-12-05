#include <iostream>
#include "RHI/vulkan_rhi.h"
#include "function/window_system.h"

using namespace std;

int main(int argc, char** argv) {
  Sparrow::WindowSystem windowSystem;
  windowSystem.initialize({.width = 800, .height = 600});
  Sparrow::VulkanRHI rhi;
  rhi.initialize({.windowSystem = windowSystem});
}
