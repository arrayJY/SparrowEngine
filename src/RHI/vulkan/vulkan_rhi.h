#ifndef SPARROWENGINE_VULKAN_RHI_H
#define SPARROWENGINE_VULKAN_RHI_H

#include "RHI/rhi.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

#include <memory>
#include <optional>

struct GLFWwindow;

namespace Sparrow {
class VulkanCommandBuffer;

class WindowSystem;
class VulkanRHI : public RHI {
 public:
  void initialize(const RHIInitInfo& initInfo) override;
  ~VulkanRHI() override;

 private:
  /*** Initialize ***/
  void init(WindowSystem* windowSystem);
  void createInstance();
  void setupDebugMessenger();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void createCommandPool();
  void createCommandBuffers();
  void createDescriptorPool();
  void createSyncPrimitives();

  /*** Override ***/
  /* Creation */
  void createSwapChain() override;
  void recreateSwapChain() override;
  void createSwapChainImageView() override;
  std::unique_ptr<RHIFramebuffer> createFramebuffer(
      RHIFramebufferCreateInfo& createInfo) override;
  void createFramebufferImageAndView() override;
  std::unique_ptr<RHIShader> createShaderModule(
      std::span<char> shader_code) override;
  std::unique_ptr<RHIPipeline> createGraphicsPipeline(
      const RHIGraphicsPipelineCreateInfo& createInfo) override;
  std::unique_ptr<RHIRenderPass> createRenderPass(
      const RHIRenderPassCreateInfo& createInfo) override;
  std::unique_ptr<RHIPipelineLayout> createPipelineLayout(
      const RHIPipelineLayoutCreateInfo& createInfo) override;
  std::tuple<std::unique_ptr<RHIBuffer>, std::unique_ptr<RHIDeviceMemory>>
  createBuffer(const RHIBufferCreateInfo& createInfo,
               RHIMemoryPropertyFlag properties) override;
  std::tuple<std::unique_ptr<RHIImage>, std::unique_ptr<RHIDeviceMemory>>
  createImage(const RHIImageCreateInfo& createInfo) override;
  void destoryBuffer(RHIBuffer* buffer) override;
  std::unique_ptr<RHIDescriptorSetLayout> createDescriptorSetLayout(
      RHIDescriptorSetLayoutCreateInfo& createInfo) override;
  void destoryDescriptorSetLayout(
      RHIDescriptorSetLayout* descriptorSetLayout) override;
  std::vector<std::unique_ptr<RHIDescriptorSet>> allocateDescriptorSets(
      const RHIDescriptorSetAllocateInfo& allocateInfo) override;

  /*** Update ***/
  void updateDescriptorSets(
      std::span<RHIWriteDescriptorSet> writeDescritorSets) override;

  /* Query */
  uint8_t getMaxFramesInFlight() override;
  uint8_t getCurrentFrameIndex() override;
  uint32_t getCurrentSwapChainImageIndex() override;
  RHISwapChainInfo getSwapChainInfo() override;
  RHICommandBuffer* getCurrentCommandBuffer() override;
  std::span<RHICommandBuffer> getCommandBuffers() override;

  /* Command */
  bool beginCommandBuffer(
      RHICommandBuffer* commandBuffer,
      RHICommandBufferBeginInfo* commandBufferBeginInfo) override;
  std::unique_ptr<RHICommandBuffer> beginOneTimeCommandBuffer() override;
  bool endCommandBuffer(RHICommandBuffer* commandBuffer) override;
  bool endOneTimeCommandBuffer(RHICommandBuffer* commandBuffer) override;

  void cmdBeginRenderPass(RHICommandBuffer* commandBuffer,
                          RHIRenderPassBeginInfo* beginInfo,
                          RHISubpassContents contents) override;
  void cmdEndRenderPass(RHICommandBuffer* commandBuffer) override;
  void cmdBindPipeline(RHICommandBuffer* commandBuffer,
                       RHIPipelineBindPoint bindPoint,
                       RHIPipeline* pipeline) override;
  void cmdBindVertexBuffers(RHICommandBuffer* commandBuffer,
                            uint32_t firstBinding,
                            uint32_t bindingCount,
                            RHIBuffer* const* buffers,
                            const RHIDeviceSize* offsets) override;
  void cmdBindIndexBuffer(RHICommandBuffer* commandBuffer,
                          RHIBuffer* buffer,
                          RHIDeviceSize offset,
                          RHIIndexType indexType) override;
  void cmdBindDescriptorSets(RHICommandBuffer* commandBuffer,
                             RHIPipelineBindPoint pipelineBindPoint,
                             const RHIPipelineLayout* layout,
                             uint32_t firstSet,
                             uint32_t descriptorSetCount,
                             const RHIDescriptorSet* descriptorSets,
                             uint32_t dynamicOffsetCount,
                             const uint32_t* dynamicOffsets) override;
  void cmdDraw(RHICommandBuffer* commandBuffer,
               uint32_t vertexCount,
               uint32_t instanceCount,
               uint32_t firstVertex,
               uint32_t firstInstance) override;
  void cmdDrawIndexed(RHICommandBuffer* commandBuffer,
                      uint32_t indexCount,
                      uint32_t instanceCount,
                      uint32_t firstIndex,
                      int32_t vertexOffset,
                      uint32_t firstInstance) override;
  void cmdSetViewport(RHICommandBuffer* commandBuffer,
                      uint32_t firstViewport,
                      uint32_t viewportCount,
                      const RHIViewport* pViewports) override;
  void cmdSetScissor(RHICommandBuffer* commandBuffer,
                     uint32_t firstScissor,
                     uint32_t scissorCount,
                     const RHIRect2D* pScissors) override;
  void cmdCopyBuffer(RHICommandBuffer* commandBuffer,
                     RHIBuffer* srcBuffer,
                     RHIBuffer* dstBuffer,
                     std::span<RHIBufferCopy> copyRegions) override;

  void beforePass() override;
  void waitIdle() override;
  void submitRendering() override;

  /*** Memory ***/
  void* mapMemory(RHIDeviceMemory* deviceMemory,
                  RHIDeviceSize offset,
                  RHIDeviceSize size) override;
  void unmapMemory(RHIDeviceMemory* deviceMemory) override;
  void freeMemory(RHIDeviceMemory* deviceMemory) override;

 private:
  /*** Structs ***/
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

 private:
  /*** Utils ***/
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
  uint32_t currentSwapChainImageIndex;

  // Framebuffer
  std::vector<vk::Framebuffer> framebuffers;
  vk::Format depthImageFormat;

  // Depth image
  vk::Image depthImage;
  vk::ImageView depthImageView;
  vk::DeviceMemory depthDeviceMemory;

  // Sync Primitives
  static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 3;
  uint8_t currentFrameIndex = 0;
  vk::Semaphore imageAvailableForRenderSemaphores[MAX_FRAMES_IN_FLIGHT];
  vk::Semaphore imageFinishedForPresentationSemaphores[MAX_FRAMES_IN_FLIGHT];
  vk::Fence isFrameInFlightFences[MAX_FRAMES_IN_FLIGHT];

  // Descriptor pool
  vk::DescriptorPool descriptorPool;

  // pipeline
  vk::PipelineCache graphicsPipelineCache;

  // GLFW
  GLFWwindow* window = nullptr;
  uint32_t width = 0, height = 0;
};
}  // namespace Sparrow

#endif  // SPARROWENGINE_VULKAN_RHI_H
