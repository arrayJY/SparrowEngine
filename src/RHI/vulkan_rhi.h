#ifndef SPARROWENGINE_VULKAN_RHI_H
#define SPARROWENGINE_VULKAN_RHI_H

#include "rhi.h"

#define VK_USE_PLATFORM_WIN32_KHR

#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32

#include <GLFW/glfw3native.h>
#include <optional>

namespace Sparrow {
    class VulkanRHI : public RHI {
    public:
        void initialize() override;

        ~VulkanRHI() override;

    private:
        void initGLFW();

        void createInstance();

        void setupDebugMessenger();

        void createSurface();

        void pickPhysicalDevice();

        void createLogicalDevice();

        void createCommandPool();

        void createCommandBuffers();

        void createSwapChain();

        void createImageView();

        void createFramebuffer();

    private:
        struct QueueFamilyIndices {
            std::optional<uint32_t> graphicsFamily;

            [[nodiscard]]bool isComplete() const {
                return graphicsFamily.has_value();
            }
        };

        struct SwapChainSupportDetails {
            vk::SurfaceCapabilitiesKHR capabilities;
            std::vector<vk::SurfaceFormatKHR> formats;
            std::vector<vk::PresentModeKHR> presentModes;
        };


    private:
        static bool checkValidationLayerSupport(const std::vector<const char *> &layerNames);

        static VKAPI_ATTR vk::Bool32 VKAPI_CALL
        debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                      vk::DebugUtilsMessageTypeFlagsEXT messageType,
                      const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData,
                      void *pUserData);

        VulkanRHI::QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice physicalDevice);

        static SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice device);

        static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);

        static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);

        vk::Extent2D chooseSwapExtend(const vk::SurfaceCapabilitiesKHR &capabilities);

    private:
        //Instance
        vk::Instance instance;
        std::vector<const char *> instanceExtensions;
        vk::DebugUtilsMessengerEXT debugMessenger;

        //Device and queue
        vk::PhysicalDevice gpu;
        vk::Device device;
        std::vector<vk::PhysicalDevice> physicalDevices;
        std::vector<const char *> deviceExtensions;
        vk::Queue graphicQueue;
        QueueFamilyIndices queueFamilyIndices;

        //Command pool and command buffers
        vk::CommandPool commandPool;
        std::vector<vk::CommandBuffer> commandBuffers;

        //Surface
        vk::SurfaceKHR surface;
        std::vector<vk::PresentModeKHR> presentModes;
        std::vector<vk::SurfaceFormatKHR> surfaceFormats;
        vk::SurfaceCapabilitiesKHR surfaceCapabilities;
        vk::PresentModeKHR presentMode;
        vk::Extent2D extent;
        HWND hwnd;
        HINSTANCE hInstance;

        //Swapchain
        vk::Format format;
        vk::SwapchainKHR swapchain;
        std::vector<vk::Image> swapchainImages;
        std::vector<vk::ImageView> swapchainImagesViews;
        vk::Extent2D swapchainExtent;
        uint32_t currentImage = 0;
        uint32_t frameCount = 0;

        //Framebuffer
        std::vector<vk::Framebuffer> framebuffers;

        //GLFW
        static constexpr unsigned WIDTH = 800;
        static constexpr unsigned HEIGHT = 600;
        GLFWwindow *window;
        uint32_t width = 0, height = 0;

    };
}

#endif //SPARROWENGINE_VULKAN_RHI_H
