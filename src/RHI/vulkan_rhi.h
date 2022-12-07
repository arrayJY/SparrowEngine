#ifndef SPARROWENGINE_VULKAN_RHI_H
#define SPARROWENGINE_VULKAN_RHI_H

#include "rhi.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

#include <memory>
#include <optional>

struct GLFWwindow;

namespace Sparrow {
class WindowSystem;
class VulkanRHI : public RHI {
 public:
  void initialize(const RHIInitInfo& initInfo) override;
  ~VulkanRHI() override;

#pragma region Initialize
 private:
  void initGLFW(WindowSystem* windowSystem);
  void createInstance();
  void setupDebugMessenger();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void createCommandPool();
  void createCommandBuffers();
  void createDescriptorPool();
  void createSyncPrimitives();
  void createSwapChain() override;
  void createSwapChainImageView() override;
  void createFramebufferImageAndView() override;
#pragma endregion

#pragma region Structs
 private:
  struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    [[nodiscard]] bool isComplete() const {
      return graphicsFamily.has_value() && presentFamily.has_value();
    }
  };

  struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
  };
#pragma endregion

#pragma region Utils
 private:
  static bool checkValidationLayerSupport(
      const std::vector<const char*>& layerNames);
  static VKAPI_ATTR vk::Bool32 VKAPI_CALL
  debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                vk::DebugUtilsMessageTypeFlagsEXT messageType,
                const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void* pUserData);
  VulkanRHI::QueueFamilyIndices findQueueFamilies(
      vk::PhysicalDevice physicalDevice);
  SwapChainSupportDetails querySwapChainSupport(
      vk::PhysicalDevice physical_device);
  static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<vk::SurfaceFormatKHR>& availableFormats);
  static vk::PresentModeKHR chooseSwapPresentMode(
      const std::vector<vk::PresentModeKHR>& availablePresentModes);
  vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
  vk::Format findDepthFormat();

#pragma endregion

#pragma region Vulkan
 private:
  // Instance
  vk::Instance instance;
  std::vector<const char*> instanceExtensions;
  vk::DebugUtilsMessengerEXT debugMessenger;

  // Device and queue
  vk::PhysicalDevice gpu;
  vk::Device device;
  std::vector<vk::PhysicalDevice> physicalDevices;
  std::vector<const char*> deviceExtensions;
  vk::Queue presentQueue;
  QueueFamilyIndices queueFamilyIndices;

  // Command pool and command buffers
  vk::CommandPool commandPool;
  std::vector<vk::CommandBuffer> commandBuffers;

  // Surface
  vk::SurfaceKHR surface;
  vk::PresentModeKHR presentMode;

  // Swapchain
  vk::SwapchainKHR swapChain;
  std::vector<vk::Image> swapChainImages;
  vk::SurfaceFormatKHR swapChainImageFormat;
  vk::Extent2D swapChainExtent;
  std::vector<vk::ImageView> swapChainImagesViews;

  // Framebuffer
  std::vector<vk::Framebuffer> framebuffers;
  vk::Format depthImageFormat;

  // Depth image
  vk::Image depthImage;
  vk::ImageView depthImageView;
  vk::DeviceMemory depthDeviceMemory;

  // Sync Primitives
  static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 3;
  vk::Semaphore imageAvailableForRenderSemaphores[MAX_FRAMES_IN_FLIGHT];
  vk::Semaphore imageFinishedForPresentationSemaphores[MAX_FRAMES_IN_FLIGHT];
  vk::Fence isFrameInFlightFences[MAX_FRAMES_IN_FLIGHT];

  // Descriptor pool
  vk::DescriptorPool descriptorPool;

#pragma endregion

#pragma region GLFW
  // GLFW
  static constexpr unsigned WIDTH = 800;
  static constexpr unsigned HEIGHT = 600;
  GLFWwindow* window = nullptr;
  uint32_t width = 0, height = 0;
#pragma endregion
};
}  // namespace Sparrow

#endif  // SPARROWENGINE_VULKAN_RHI_H
