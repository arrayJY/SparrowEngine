#include "vulkan_rhi.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <format>
#include <ranges>

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
        createSwapChain();
        createImageView();
        createFramebuffer();
    }

    VulkanRHI::~VulkanRHI() {

    }

    void VulkanRHI::initGLFW() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window =
                glfwCreateWindow(WIDTH, HEIGHT, "Sparrow Engine", nullptr, nullptr);
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
                .setApiVersion(VK_VERSION_1_3);

        static const std::vector<const char *> validationLayers{
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
        auto GetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        vk::DispatchLoaderDynamic dispatch(instance, GetInstanceProcAddr);
        if (instance.createDebugUtilsMessengerEXT(&debugMessengerInfo, nullptr,
                                                  &debugMessenger,
                                                  dispatch) != vk::Result::eSuccess) {
            throw std::runtime_error("Create debug util messenger Failed");
        };
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
        queueFamilyProps = gpu.getQueueFamilyProperties();

        const auto supports =
                std::views::iota((std::size_t) 0, queueFamilyProps.size()) |
                std::views::transform([this](auto i) {
                    auto support = gpu.getSurfaceSupportKHR(i, surface);
                    return support == VK_TRUE &&
                           queueFamilyProps[i].queueFlags & vk::QueueFlagBits::eGraphics;
                });
        const auto graphicProp = std::find(supports.begin(), supports.end(), true);
        bool found = graphicProp != supports.end();
        if (!found) {
            throw std::runtime_error("No found surface support queue.");
        }
        graphicIndex =
                static_cast<uint32_t>(std::distance(supports.begin(), graphicProp));

        auto feature = vk::PhysicalDeviceFeatures().setGeometryShader(VK_TRUE);

        float priorities[] = {0.0f};
        deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        auto deviceQueueInfo = vk::DeviceQueueCreateInfo()
                .setQueueCount(1)
                .setPQueuePriorities(priorities)
                .setQueueFamilyIndex(graphicIndex);

        auto deviceInfo = vk::DeviceCreateInfo()
                .setQueueCreateInfoCount(1)
                .setPQueueCreateInfos(&deviceQueueInfo)
                .setPEnabledExtensionNames(deviceExtensions)
                .setPEnabledFeatures(&feature);

        device = gpu.createDevice(deviceInfo);
        graphicQueue = device.getQueue(graphicIndex, 0);

        surfaceFormats = gpu.getSurfaceFormatsKHR(surface);
        if (surfaceFormats.empty()) {
            throw std::runtime_error("Get surface formats khr error");
        }

        presentModes = gpu.getSurfacePresentModesKHR(surface);
        if (presentModes.empty()) {
            throw std::runtime_error("Get present modes khr error");
        }

        surfaceCapabilities = gpu.getSurfaceCapabilitiesKHR(surface);
    }

    void VulkanRHI::createCommandPool() {

    }

    void VulkanRHI::createCommandBuffers() {

    }

    void VulkanRHI::createSwapChain() {
        format = surfaceFormats[0].format;
        frameCount = surfaceCapabilities.minImageCount;
        auto swapchainInfo =
                vk::SwapchainCreateInfoKHR()
                        .setSurface(surface)
                        .setImageFormat(format)
                        .setMinImageCount(frameCount)
                        .setImageExtent(vk::Extent2D(width, height))
                        .setPresentMode(vk::PresentModeKHR::eFifo)
                        .setImageSharingMode(vk::SharingMode::eExclusive)
                        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
                        .setImageColorSpace(surfaceFormats[0].colorSpace)
                        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
                        .setImageArrayLayers(1)
                        .setClipped(true);
        swapchain = device.createSwapchainKHR(swapchainInfo);
    }

    void VulkanRHI::createImageView() {
        swapchainImages = device.getSwapchainImagesKHR(swapchain);
        frameCount = swapchainImages.size();

        swapchainIamgesViews.resize(frameCount);
        for (uint32_t i = 0; i < frameCount; i++) {
            auto createImageViewInfo =
                    vk::ImageViewCreateInfo()
                            .setViewType(vk::ImageViewType::e2D)
                            .setSubresourceRange(vk::ImageSubresourceRange(
                                    vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1))
                            .setFormat(format)
                            .setImage(swapchainImages[i])
                            .setComponents(
                                    vk::ComponentMapping(vk::ComponentSwizzle::eIdentity,
                                                         vk::ComponentSwizzle::eIdentity,
                                                         vk::ComponentSwizzle::eIdentity,
                                                         vk::ComponentSwizzle::eIdentity));
            swapchainIamgesViews[i] = device.createImageView(createImageViewInfo);
        }
    }

    void VulkanRHI::createFramebuffer() {

    }

    bool VulkanRHI::checkValidationLayerSupport(const std::vector<const char *> &layerNames) {
        const std::vector<vk::LayerProperties> properties =
                vk::enumerateInstanceLayerProperties();
        return std::all_of(
                layerNames.begin(), layerNames.end(), [&properties](auto layerName) {
                    return std::any_of(properties.begin(), properties.end(),
                                       [layerName](const vk::LayerProperties &prop) {
                                           return prop.layerName == std::string(layerName);
                                       });
                });
    }

    vk::Bool32 VulkanRHI::debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                        vk::DebugUtilsMessageTypeFlagsEXT messageType,
                                        const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
        using DebugSeverityFlag = vk::DebugUtilsMessageSeverityFlagBitsEXT;
        switch (messageSeverity) {
            case DebugSeverityFlag::eInfo:
                std::cout << "[validation layer (Info)]" << pCallbackData->pMessage << "\n";
                break;
            case DebugSeverityFlag::eVerbose:
                std::cout << "[validation layer (Verbose)]" << pCallbackData->pMessage << "\n";
                break;
            case DebugSeverityFlag::eWarning:
                std::cerr << "[validation layer (Warning)]" << pCallbackData->pMessage << "\n";
                break;
            default:
                std::cerr << "[validation layer (Error)]" << pCallbackData->pMessage << "\n";
                break;
        }
        return VK_FALSE;
    }



};
