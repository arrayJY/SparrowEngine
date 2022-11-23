#define NOMINMAX

#include "vulkan_rhi.h"
#include <algorithm>
#include <array>
#include <format>
#include <iostream>
#include <limits>
#include <ranges>
#include <set>
#include <vector>
#include "vulkan_utils.h"

namespace Sparrow {
#ifdef NDEBUG

static constexpr bool enableValidationLayers = false;

#else
static constexpr bool enableValidationLayers = true;
#endif

void VulkanRHI::initialize() {
  initGLFW();
  createInstance();
  setupDebugMessenger();
  createSurface();
  pickPhysicalDevice();
  createLogicalDevice();
  createCommandPool();
  createCommandBuffers();
  createSyncPrimitives();
  createSwapChain();
  createSwapChainImageView();
  createFramebufferImageAndView();
}

VulkanRHI::~VulkanRHI() {}

void VulkanRHI::initGLFW() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  window = glfwCreateWindow(WIDTH, HEIGHT, "Sparrow Engine", nullptr, nullptr);
  hwnd = glfwGetWin32Window(window);
  hInstance = GetModuleHandle(nullptr);
  int w, h;
  glfwGetWindowSize(window, &w, &h);
  width = w, height = h;
}

void VulkanRHI::createInstance() {
  auto appInfo = vk::ApplicationInfo()
                     .setPApplicationName("Sparrow")
                     .setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
                     .setPEngineName("Sparrow Engine")
                     .setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
                     .setApiVersion(VK_API_VERSION_1_3);

  static const std::vector<const char*> validationLayers{
      "VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_monitor"};

  if (enableValidationLayers &&
      !checkValidationLayerSupport(validationLayers)) {
    throw std::runtime_error("Not support validation layer");
  }

  instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
  instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
  if (enableValidationLayers) {
    instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  auto createInfo = vk::InstanceCreateInfo()
                        .setPApplicationInfo(&appInfo)
                        .setPEnabledExtensionNames(validationLayers)
                        .setPEnabledExtensionNames(instanceExtensions);

  instance = vk::createInstance(createInfo);
}

void VulkanRHI::setupDebugMessenger() {
  using SeverityFlag = vk::DebugUtilsMessageSeverityFlagBitsEXT;
  using MessageType = vk::DebugUtilsMessageTypeFlagBitsEXT;

  auto callbackFunc =
      reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(debugCallback);
  auto debugMessengerInfo =
      vk::DebugUtilsMessengerCreateInfoEXT()
          .setMessageSeverity(SeverityFlag::eInfo | SeverityFlag::eVerbose |
                              SeverityFlag::eWarning | SeverityFlag::eError)
          .setMessageType(MessageType::eGeneral | MessageType::eValidation |
                          MessageType::ePerformance)
          .setPfnUserCallback(callbackFunc)
          .setPUserData(nullptr);

  vk::DynamicLoader dl;
  auto GetInstanceProcAddr =
      dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
  vk::DispatchLoaderDynamic dispatch(instance, GetInstanceProcAddr);
  if (instance.createDebugUtilsMessengerEXT(&debugMessengerInfo, nullptr,
                                            &debugMessenger,
                                            dispatch) != vk::Result::eSuccess) {
    throw std::runtime_error("Create debug util messenger Failed");
  }
}

void VulkanRHI::createSurface() {
  auto surfaceInfo =
      vk::Win32SurfaceCreateInfoKHR().setHwnd(hwnd).setHinstance(hInstance);
  surface = instance.createWin32SurfaceKHR(surfaceInfo);
}

void VulkanRHI::pickPhysicalDevice() {
  physicalDevices = instance.enumeratePhysicalDevices();
  if (physicalDevices.empty()) {
    throw std::runtime_error("No physical device.");
  } else {
    gpu = physicalDevices[0];
  }
}

void VulkanRHI::createLogicalDevice() {
  queueFamilyIndices = findQueueFamilies(gpu);
  if (!queueFamilyIndices.isComplete()) {
    throw std::runtime_error("Find queue families failed.");
  }
  auto graphicIndex = queueFamilyIndices.graphicsFamily.value();
  auto feature = vk::PhysicalDeviceFeatures().setGeometryShader(VK_TRUE);
  deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {
      queueFamilyIndices.graphicsFamily.value(),
      queueFamilyIndices.presentFamily.value()};
  float priorities = 1.0f;
  for (auto queueFamilyIndex : uniqueQueueFamilies) {
    auto deviceQueueInfo = vk::DeviceQueueCreateInfo()
                               .setQueueCount(1)
                               .setPQueuePriorities(&priorities)
                               .setQueueFamilyIndex(queueFamilyIndex);
    queueCreateInfos.push_back(deviceQueueInfo);
  }

  auto deviceInfo = vk::DeviceCreateInfo()
                        .setQueueCreateInfos(queueCreateInfos)
                        .setPEnabledExtensionNames(deviceExtensions)
                        .setPEnabledFeatures(&feature);

  device = gpu.createDevice(deviceInfo);
  presentQueue = device.getQueue(queueFamilyIndices.presentFamily.value(), 0);
  depthImageFormat = findDepthFormat();
}

void VulkanRHI::createCommandPool() {
  auto commandPoolInfo =
      vk::CommandPoolCreateInfo()
          .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
          .setQueueFamilyIndex(queueFamilyIndices.graphicsFamily.value());
  commandPool = device.createCommandPool(commandPoolInfo);
}

void VulkanRHI::createCommandBuffers() {
  auto allocInfo = vk::CommandBufferAllocateInfo()
                       .setCommandPool(commandPool)
                       .setLevel(vk::CommandBufferLevel::ePrimary)
                       .setCommandBufferCount(1);
  commandBuffers = device.allocateCommandBuffers(allocInfo);
}

void VulkanRHI::createSyncPrimitives() {
  auto semaphoreInfo = vk::SemaphoreCreateInfo();
  auto fenceInfo =
      vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);
  for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    imageAvailableForRenderSemaphores[i] =
        device.createSemaphore(semaphoreInfo);
    imageFinishedForPresentationSemaphores[i] =
        device.createSemaphore(semaphoreInfo);
    isFrameInFlightFences[i] = device.createFence(fenceInfo);
  }
}

void VulkanRHI::createSwapChain() {
  auto swapChainSupport = querySwapChainSupport(gpu);
  auto& capabilities = swapChainSupport.capabilities;

  presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
  swapChainImageFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
  swapChainExtent = chooseSwapExtent(capabilities);

  uint32_t imageCount = capabilities.minImageCount + 1;
  if (capabilities.maxImageCount > 0 &&
      imageCount > capabilities.maxImageCount) {
    imageCount = capabilities.maxImageCount;
  }

  auto& indices = queueFamilyIndices;
  auto graphicsIsSameAsPresent =
      indices.graphicsFamily == indices.presentFamily;

  uint32_t queueIndices[] = {indices.graphicsFamily.value(),
                             indices.presentFamily.value()};
  auto imageSharingMode = graphicsIsSameAsPresent
                              ? vk::SharingMode::eExclusive
                              : vk::SharingMode::eConcurrent;
  auto queueFamilyIndexCount = graphicsIsSameAsPresent ? 0 : 2;
  auto pQueueIndices = graphicsIsSameAsPresent ? nullptr : queueIndices;

  auto swapChainInfo =
      vk::SwapchainCreateInfoKHR()
          .setSurface(surface)
          .setMinImageCount(imageCount)
          .setImageFormat(swapChainImageFormat.format)
          .setImageColorSpace(swapChainImageFormat.colorSpace)
          .setImageExtent(swapChainExtent)
          .setImageArrayLayers(1)
          .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
          .setImageSharingMode(imageSharingMode)
          .setQueueFamilyIndexCount(queueFamilyIndexCount)
          .setPQueueFamilyIndices(pQueueIndices)
          .setPreTransform(swapChainSupport.capabilities.currentTransform)
          .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
          .setPresentMode(presentMode)
          .setClipped(true);
  swapChain = device.createSwapchainKHR(swapChainInfo);
}

void VulkanRHI::createSwapChainImageView() {
  swapChainImages = device.getSwapchainImagesKHR(swapChain);
  auto frameCount = swapChainImages.size();

  swapChainImagesViews.resize(frameCount);
  for (uint32_t i = 0; i < frameCount; i++) {
    auto createImageViewInfo =
        vk::ImageViewCreateInfo()
            .setImage(swapChainImages[i])
            .setViewType(vk::ImageViewType::e2D)
            .setFormat(swapChainImageFormat.format)
            .setComponents(
                vk::ComponentMapping(vk::ComponentSwizzle::eIdentity,
                                     vk::ComponentSwizzle::eIdentity,
                                     vk::ComponentSwizzle::eIdentity,
                                     vk::ComponentSwizzle::eIdentity))
            .setSubresourceRange(vk::ImageSubresourceRange(
                vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
    swapChainImagesViews[i] = device.createImageView(createImageViewInfo);
  }
}

void VulkanRHI::createFramebufferImageAndView() {
  VulkanUtils::createImage(gpu, device, swapChainExtent.width,
                           swapChainExtent.height, depthImageFormat,
                           vk::ImageTiling::eOptimal,
                           vk::ImageUsageFlagBits::eInputAttachment |
                               vk::ImageUsageFlagBits::eDepthStencilAttachment |
                               vk::ImageUsageFlagBits::eTransferSrc,
                           vk::MemoryPropertyFlagBits::eDeviceLocal,
                           std::nullopt, 1, 1, depthImage, depthDeviceMemory);
  depthImageView = VulkanUtils::createImageView(
      device, depthImage, depthImageFormat, vk::ImageAspectFlagBits::eDepth,
      vk::ImageViewType::e2D, 1, 1);
}

bool VulkanRHI::checkValidationLayerSupport(
    const std::vector<const char*>& layerNames) {
  const std::vector<vk::LayerProperties> properties =
      vk::enumerateInstanceLayerProperties();
  return std::all_of(
      layerNames.begin(), layerNames.end(), [&properties](auto layerName) {
        return std::any_of(properties.begin(), properties.end(),
                           [layerName](const vk::LayerProperties& prop) {
                             return prop.layerName == std::string(layerName);
                           });
      });
}

vk::Bool32 VulkanRHI::debugCallback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    vk::DebugUtilsMessageTypeFlagsEXT messageType,
    const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
  using DebugSeverityFlag = vk::DebugUtilsMessageSeverityFlagBitsEXT;
  switch (messageSeverity) {
    case DebugSeverityFlag::eInfo:
      std::cerr << "[validation layer (Info)]" << pCallbackData->pMessage
                << "\n";
      break;
    case DebugSeverityFlag::eVerbose:
      /*
    std::cout << "[validation layer (Verbose)]" << pCallbackData->pMessage
              << "\n";
              */
      break;
    case DebugSeverityFlag::eWarning:
      std::cerr << "[validation layer (Warning)]" << pCallbackData->pMessage
                << "\n";
      break;
    default:
      std::cerr << "[validation layer (Error)]" << pCallbackData->pMessage
                << "\n";
      break;
  }
  return VK_FALSE;
}

VulkanRHI::QueueFamilyIndices VulkanRHI::findQueueFamilies(
    vk::PhysicalDevice physicalDevice) {
  auto queueFamilyProp = physicalDevice.getQueueFamilyProperties();

  for (auto i = 0; i < queueFamilyProp.size(); i++) {
    auto support = physicalDevice.getSurfaceSupportKHR(i, surface);
    if (!queueFamilyIndices.graphicsFamily.has_value() &&
        queueFamilyProp[i].queueFlags & vk::QueueFlagBits::eGraphics) {
      queueFamilyIndices.graphicsFamily = i;
    }
    if (!queueFamilyIndices.presentFamily.has_value() && support) {
      queueFamilyIndices.presentFamily = i;
    }
  }

  return queueFamilyIndices;
}

VulkanRHI::SwapChainSupportDetails VulkanRHI::querySwapChainSupport(
    vk::PhysicalDevice physical_device) {
  return SwapChainSupportDetails{
      .capabilities = physical_device.getSurfaceCapabilitiesKHR(surface),
      .formats = physical_device.getSurfaceFormatsKHR(surface),
      .presentModes = physical_device.getSurfacePresentModesKHR(surface),
  };
}

vk::SurfaceFormatKHR VulkanRHI::chooseSwapSurfaceFormat(
    const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
  for (const auto& availableFormat : availableFormats) {
    if (availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
        availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
      return availableFormat;
    }
  }
  return availableFormats[0];
}

vk::PresentModeKHR VulkanRHI::chooseSwapPresentMode(
    const std::vector<vk::PresentModeKHR>& availablePresentModes) {
  for (const auto& availablePresentMode : availablePresentModes) {
    if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
      return availablePresentMode;
    }
  }
  return vk::PresentModeKHR::eFifo;
}

vk::Extent2D VulkanRHI::chooseSwapExtent(
    const vk::SurfaceCapabilitiesKHR& capabilities) {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    auto actualExtent = vk::Extent2D().setWidth(w).setHeight(h);
    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);
    return actualExtent;
  }
}
vk::Format VulkanRHI::findDepthFormat() {
  const auto candidates = {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint,
                           vk::Format::eD24UnormS8Uint};
  for (auto format : candidates) {
    auto props = gpu.getFormatProperties(format);
    auto depthFeature = vk::FormatFeatureFlagBits::eDepthStencilAttachment;
    if ((props.optimalTilingFeatures & depthFeature) == depthFeature) {
      return format;
    }
  }
  return vk::Format::eUndefined;
}

}  // namespace Sparrow
