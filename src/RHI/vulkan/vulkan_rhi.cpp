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
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "RHI/vulkan/vulkan_rhi_resource.h"
#include "function/window_system.h"
#include "utils/log.h"
#include "vulkan_utils.h"

namespace Sparrow {
#define LOG_IF_NOT_SUCCESS(RET, TEXT)  \
  do {                                 \
    if ((RET) != vk::Result::eSuccess) \
      LOG_ERROR((TEXT));               \
  } while (0)

#ifdef NDEBUG

static constexpr bool enableValidationLayers = false;

#else
static constexpr bool enableValidationLayers = true;
#endif

void VulkanRHI::initialize(const RHIInitInfo& initInfo) {
  init(initInfo.windowSystem.get());
  createInstance();
  setupDebugMessenger();
  createSurface();
  pickPhysicalDevice();
  createLogicalDevice();
  createCommandPool();
  createCommandBuffers();
  createDescriptorPool();
  createSyncPrimitives();
  createSwapChain();
  createSwapChainImageView();
  createFramebufferImageAndView();
}

VulkanRHI::~VulkanRHI() {}

void VulkanRHI::init(WindowSystem* windowSystem) {
  window = windowSystem->getWindow();
  auto [w, h] = windowSystem->getWindowSize();
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
  auto _surface = surface.operator VkSurfaceKHR();
  glfwCreateWindowSurface(instance, window, nullptr, &_surface);
  surface = _surface;
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
                       .setCommandBufferCount(MAX_FRAMES_IN_FLIGHT);
  commandBuffers = device.allocateCommandBuffers(allocInfo);
}

void VulkanRHI::createDescriptorPool() {
  const auto poolSize =
      vk::DescriptorPoolSize().setDescriptorCount(MAX_FRAMES_IN_FLIGHT);
  const auto poolCreateInfo = vk::DescriptorPoolCreateInfo()
                                  .setPoolSizeCount(1)
                                  .setPPoolSizes(&poolSize)
                                  .setMaxSets(MAX_FRAMES_IN_FLIGHT);
  descriptorPool = device.createDescriptorPool(poolCreateInfo);
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

void VulkanRHI::recreateSwapChain() {
  int _width, _height;
  glfwGetFramebufferSize(window, &_width, &_height);
  while (_width == 0 || _height == 0) {
    glfwGetFramebufferSize(window, &_width, &_height);
    glfwWaitEvents();
  }
  width = _width, height = _height;

  device.waitIdle();

  if (device.waitForFences(1, &isFrameInFlightFences[currentFrameIndex],
                           VK_TRUE, UINT64_MAX) != vk::Result::eSuccess) {
    LOG_ERROR("WaitForFences failed.")
    return;
  }

  device.destroyImageView(depthImageView);
  device.destroyImage(depthImage);
  for (auto imageView : swapChainImagesViews) {
    device.destroyImageView(imageView);
  }
  device.destroySwapchainKHR(swapChain);

  createSwapChain();
  createSwapChainImageView();
  createFramebufferImageAndView();
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

std::unique_ptr<RHIFramebuffer> VulkanRHI::createFramebuffer(
    RHIFramebufferCreateInfo& createInfo) {
  auto frameBufferCreateInfo =
      vk::FramebufferCreateInfo()
          .setRenderPass(GetResource<VulkanRenderPass>(createInfo.renderPass))
          .setAttachmentCount(createInfo.attachmentCount)
          .setPAttachments(Cast<vk::ImageView>(createInfo.attachments) +
                           createInfo.attachmentsOffset)
          .setWidth(createInfo.width)
          .setHeight(createInfo.height)
          .setLayers(createInfo.layers);
  vk::Framebuffer vkFramebuffer;
  if (device.createFramebuffer(&frameBufferCreateInfo, nullptr,
                               &vkFramebuffer) != vk::Result::eSuccess) {
    return nullptr;
  }
  auto framebuffer = std::make_unique<VulkanFramebuffer>();
  framebuffer->setResource(vkFramebuffer);
  return framebuffer;
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

std::unique_ptr<RHIShader> VulkanRHI::createShaderModule(
    std::span<char> shader_code) {
  auto shaderModuleCreateInfo =
      vk::ShaderModuleCreateInfo()
          .setCodeSize(shader_code.size())
          .setPCode(reinterpret_cast<const uint32_t*>(shader_code.data()));

  vk::ShaderModule vkShaderModule;
  if (device.createShaderModule(&shaderModuleCreateInfo, nullptr,
                                &vkShaderModule) != vk::Result::eSuccess) {
    LOG_ERROR("CreateShaderModule failed.\n")
    return nullptr;
  }
  auto vulkanShader = std::make_unique<VulkanShader>();
  vulkanShader->setResource(vkShaderModule);
  return vulkanShader;
}

std::unique_ptr<RHIPipeline> VulkanRHI::createGraphicsPipeline(
    const RHIGraphicsPipelineCreateInfo& createInfo) {
  const auto shaderStageCount = createInfo.stageCount;
  const auto& rhiShaderStages = createInfo.shaderStageCreateInfo;
  const auto& rhiDynamicState = createInfo.dynamicStateCreateInfo;
  const auto& rhiVertexInputState = createInfo.vertexInputStateCreateInfo;
  const auto& rhiInputAssemblyState = createInfo.inputAssemblyStateCreateInfo;
  const auto& rhiViewportState = createInfo.viewportStateCreateInfo;
  const auto& rhiRasterizationState = createInfo.rasterizationStateCreateInfo;
  const auto& rhiMultisampleState = createInfo.multisampleStateCreateInfo;
  const auto& rhiDepthStencilState = createInfo.depthStencilStateCreateInfo;
  const auto& rhiColorBlendState = createInfo.colorBlendStateCreateInfo;

  std::vector<vk::PipelineShaderStageCreateInfo> shaderStageCreateInfos(
      shaderStageCount);

  for (auto i = 0; i < shaderStageCount; i++) {
    const auto& rhiShaderStage = rhiShaderStages[i];
    auto& shaderStageCreateInfo = shaderStageCreateInfos[i];

    shaderStageCreateInfo
        .setStage(Cast<vk::ShaderStageFlagBits>(rhiShaderStage.stage))
        .setPName(rhiShaderStage.name)
        .setModule(GetResource<VulkanShader>(rhiShaderStage.module))
        .setPSpecializationInfo(
            Cast<vk::SpecializationInfo>(rhiShaderStage.specializationInfo));
  }

  auto dynamicStateCreateInfo =
      vk::PipelineDynamicStateCreateInfo()
          .setDynamicStateCount(rhiDynamicState->dynamicStateCount)
          .setPDynamicStates(
              Cast<vk::DynamicState>(rhiDynamicState->dynamicStates));
  auto vertexInputStateCreateInfo =
      vk::PipelineVertexInputStateCreateInfo()
          .setVertexBindingDescriptionCount(
              rhiVertexInputState->vertexBindingDescriptionCount)
          .setPVertexBindingDescriptions(
              Cast<vk::VertexInputBindingDescription>(
                  rhiVertexInputState->vertexBindingDescriptions))
          .setVertexAttributeDescriptionCount(
              rhiVertexInputState->vertexAttributeDescriptionCount)
          .setPVertexAttributeDescriptions(
              Cast<vk::VertexInputAttributeDescription>(
                  rhiVertexInputState->vertexAttributeDescriptions));
  auto inputAssemblyStateCreateInfo =
      vk::PipelineInputAssemblyStateCreateInfo()
          .setTopology(
              Cast<vk::PrimitiveTopology>(rhiInputAssemblyState->topology))
          .setPrimitiveRestartEnable(
              rhiInputAssemblyState->primitiveRestartEnabled);

  auto viewportStateCreateInfo =
      vk::PipelineViewportStateCreateInfo()
          .setViewportCount(rhiViewportState->viewportCount)
          .setPViewports(Cast<vk::Viewport>(rhiViewportState->viewports))
          .setScissorCount(rhiViewportState->scissorCount)
          .setPScissors(Cast<vk::Rect2D>(rhiViewportState->scissors));

  auto rasterizationStateCreateInfo =
      vk::PipelineRasterizationStateCreateInfo()
          .setDepthClampEnable(rhiRasterizationState->depthClampEnable)
          .setRasterizerDiscardEnable(
              rhiRasterizationState->rasterizerDiscardEnable)
          .setPolygonMode(
              Cast<vk::PolygonMode>(rhiRasterizationState->polygonMode))
          .setLineWidth(rhiRasterizationState->lineWidth)
          .setCullMode(
              Cast<vk::CullModeFlagBits>(rhiRasterizationState->cullMode))
          .setFrontFace(Cast<vk::FrontFace>(rhiRasterizationState->frontFace))
          .setDepthBiasEnable(rhiRasterizationState->depthBiasEnable)
          .setDepthBiasConstantFactor(
              rhiRasterizationState->depthBiasConstantFactor)
          .setDepthBiasClamp(rhiRasterizationState->depthBiasClamp)
          .setDepthBiasSlopeFactor(rhiRasterizationState->depthBiasSlopeFactor);

  auto multisampleStateCreateInfo =
      vk::PipelineMultisampleStateCreateInfo()
          .setSampleShadingEnable(rhiMultisampleState->sampleShadingEnable)
          .setRasterizationSamples(Cast<vk::SampleCountFlagBits>(
              rhiMultisampleState->rasterizationSamples))
          .setMinSampleShading(rhiMultisampleState->minSampleShading)
          .setPSampleMask(rhiMultisampleState->sampleMask)
          .setAlphaToCoverageEnable(rhiMultisampleState->alphaToCoverageEnable)
          .setAlphaToOneEnable(rhiMultisampleState->alphaToOneEnable);

  auto depthStencilStateCreateInfo = vk::PipelineDepthStencilStateCreateInfo();

  if (rhiDepthStencilState) {
    depthStencilStateCreateInfo
        .setDepthTestEnable(rhiDepthStencilState->depthTestEnable)
        .setDepthWriteEnable(rhiDepthStencilState->depthWriteEnable)
        .setDepthCompareOp(
            Cast<vk::CompareOp>(rhiDepthStencilState->depthCompareOp))
        .setDepthBoundsTestEnable(rhiDepthStencilState->depthBoundsTestEnable)
        .setStencilTestEnable(rhiDepthStencilState->stencilTestEnable)
        .setFront(Cast<vk::StencilOpState>(rhiDepthStencilState->front))
        .setBack(Cast<vk::StencilOpState>(rhiDepthStencilState->back))
        .setMinDepthBounds(rhiDepthStencilState->minDepthBounds)
        .setMaxDepthBounds(rhiDepthStencilState->maxDepthBounds);
  }

  auto colorBlendStateCreateInfo =
      vk::PipelineColorBlendStateCreateInfo()
          .setLogicOpEnable(rhiColorBlendState->logicOpEnable)
          .setLogicOp(Cast<vk::LogicOp>(rhiColorBlendState->logicOp))
          .setAttachmentCount(rhiColorBlendState->attachmentCount)
          .setPAttachments(Cast<vk::PipelineColorBlendAttachmentState>(
              rhiColorBlendState->attachments))
          .setBlendConstants(rhiColorBlendState->blendConstants);

  auto graphicsPipelineCreateInfo =
      vk::GraphicsPipelineCreateInfo()
          .setStageCount(shaderStageCount)
          .setPStages(shaderStageCreateInfos.data())
          .setPVertexInputState(&vertexInputStateCreateInfo)
          .setPInputAssemblyState(&inputAssemblyStateCreateInfo)
          .setPViewportState(&viewportStateCreateInfo)
          .setPRasterizationState(&rasterizationStateCreateInfo)
          .setPMultisampleState(&multisampleStateCreateInfo)
          .setPDepthStencilState(
              rhiDepthStencilState ? &depthStencilStateCreateInfo : nullptr)
          .setPColorBlendState(&colorBlendStateCreateInfo)
          .setPDynamicState(&dynamicStateCreateInfo)
          .setLayout(
              GetResource<VulkanPipelineLayout>(createInfo.pipelineLayout))
          .setRenderPass(GetResource<VulkanRenderPass>(createInfo.renderPass))
          .setSubpass(createInfo.subpass)
          .setBasePipelineHandle(
              createInfo.basePipelineHandle
                  ? GetResource<VulkanPipeline>(createInfo.basePipelineHandle)
                  : nullptr)
          .setBasePipelineIndex(createInfo.basePipelineIndex);

  vk::Pipeline vkGraphicsPipeline;
  if (auto pipelineCreateResult =
          device.createGraphicsPipelines(
              graphicsPipelineCache, 1, &graphicsPipelineCreateInfo, nullptr,
              &vkGraphicsPipeline) != vk::Result::eSuccess) {
    return nullptr;
  }
  auto pipeline = std::make_unique<VulkanPipeline>();
  pipeline->setResource(vkGraphicsPipeline);
  return pipeline;
}

std::unique_ptr<RHIRenderPass> VulkanRHI::createRenderPass(
    const RHIRenderPassCreateInfo& createInfo) {
  auto renderPassCreateInfo =
      vk::RenderPassCreateInfo()
          .setAttachmentCount(createInfo.attachmentCount)
          .setPAttachments(
              Cast<vk::AttachmentDescription>(createInfo.attachments))
          .setSubpassCount(createInfo.subpassCount)
          .setPSubpasses(Cast<vk::SubpassDescription>(createInfo.subpasses));

  vk::RenderPass vkRenderPass;
  if (device.createRenderPass(&renderPassCreateInfo, nullptr, &vkRenderPass) !=
      vk::Result::eSuccess) {
    return nullptr;
  }
  auto renderPass = std::make_unique<VulkanRenderPass>();
  renderPass->setResource(vkRenderPass);
  return renderPass;
}

std::unique_ptr<RHIPipelineLayout> VulkanRHI::createPipelineLayout(
    const RHIPipelineLayoutCreateInfo& createInfo) {
  auto pipelineLayoutCreateInfo =
      vk::PipelineLayoutCreateInfo()
          .setSetLayoutCount(createInfo.setLayoutCount)
          .setPSetLayouts(Cast<vk::DescriptorSetLayout>(createInfo.setLayouts))
          .setPushConstantRangeCount(createInfo.pushConstantRangeCount)
          .setPPushConstantRanges(
              Cast<vk::PushConstantRange>(createInfo.pushConstantRanges));

  vk::PipelineLayout vkPipelineLayout;
  if (device.createPipelineLayout(&pipelineLayoutCreateInfo, nullptr,
                                  &vkPipelineLayout) != vk::Result::eSuccess) {
    return nullptr;
  }

  auto pipelineLayout = std::make_unique<VulkanPipelineLayout>();
  pipelineLayout->setResource(vkPipelineLayout);
  return pipelineLayout;
}

std::tuple<std::unique_ptr<RHIBuffer>, std::unique_ptr<RHIDeviceMemory>>
VulkanRHI::createBuffer(const RHIBufferCreateInfo& createInfo,
                        RHIMemoryPropertyFlag properties) {
  auto [vkBuffer, vkDeviceMemory] =
      VulkanUtils::createBuffer(gpu, device, createInfo, properties);

  auto buffer = std::make_unique<VulkanBuffer>();
  auto deviceMemory = std::make_unique<VulkanDeviceMemory>();
  buffer->setResource(vkBuffer);
  deviceMemory->setResource(vkDeviceMemory);

  return std::make_tuple(std::move(buffer), std::move(deviceMemory));
}

std::tuple<std::unique_ptr<RHIImage>, std::unique_ptr<RHIDeviceMemory>>
VulkanRHI::createImage(const RHIImageCreateInfo& createInfo) {
  vk::Image vkImage;
  vk::DeviceMemory vkDeviceMemory;
  VulkanUtils::createImage(
      gpu, device, createInfo.width, createInfo.height,
      Cast<vk::Format>(createInfo.format),
      Cast<vk::ImageTiling>(createInfo.tiling),
      Cast<vk::ImageUsageFlags>(createInfo.imageUsageFlags),
      Cast<vk::MemoryPropertyFlags>(createInfo.memoryPropertyFlags),
      Cast<vk::ImageCreateFlags>(createInfo.imageCreateFlags),
      createInfo.arrayLayers, createInfo.mipLevels, vkImage, vkDeviceMemory);
  auto image = std::make_unique<VulkanImage>();
  auto deviceMemory = std::make_unique<VulkanDeviceMemory>();
  image->setResource(vkImage);
  deviceMemory->setResource(vkDeviceMemory);
  return std::make_tuple(std::move(image), std::move(deviceMemory));
}

std::tuple<std::unique_ptr<RHIImage>,
           std::unique_ptr<RHIImageView>,
           std::unique_ptr<RHIDeviceMemory>>
VulkanRHI::createImageAndCopyData(const RHIImageCreateInfo& createInfo,
                                  void* data,
                                  size_t dataSize) {
  auto stagingBufferCreateInfo = RHIBufferCreateInfo{
      .size = dataSize,
      .usage = RHIBufferUsageFlag::TransferSrc,
      .sharingMode = RHISharingMode::Exclusive,
  };

  auto [stagingBuffer, stagingBufferMemory] = VulkanUtils::createBuffer(
      gpu, device, stagingBufferCreateInfo,
      RHIMemoryPropertyFlag::HostVisible | RHIMemoryPropertyFlag::HostCoherent);

  void* stagingBufferMappedMemory =
      device.mapMemory(stagingBufferMemory, 0, dataSize);
  std::memcpy(stagingBufferMappedMemory, data, dataSize);
  device.unmapMemory(stagingBufferMemory);

  auto [image, imageMemory] = createImage(createInfo);
  auto vkImage = GetResource<VulkanImage>(image.get());

  VulkanUtils::transitionImageLayout(this, vkImage, vk::Format::eR8G8B8A8Srgb,
                                     vk::ImageLayout::eUndefined,
                                     vk::ImageLayout::eTransferDstOptimal);

  VulkanUtils::copyBufferToImage(this, stagingBuffer, vkImage, createInfo.width,
                                 createInfo.height);

  VulkanUtils::transitionImageLayout(
      this, GetResource<VulkanImage>(image.get()), vk::Format::eR8G8B8A8Srgb,
      vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eReadOnlyOptimal);

  auto vkImageView = VulkanUtils::createImageView(
      device, vkImage, Cast<vk::Format>(createInfo.format),
      vk::ImageAspectFlagBits::eColor, vk::ImageViewType::e2D, 1, 0);
  auto imageView = std::make_unique<VulkanImageView>();
  imageView->setResource(vkImageView);

  device.destroyBuffer(stagingBuffer);
  device.freeMemory(stagingBufferMemory);

  return std::make_tuple(std::move(image), std::move(imageView), std::move(imageMemory));
}

void VulkanRHI::destoryBuffer(RHIBuffer* buffer) {
  device.destroyBuffer(GetResource<VulkanBuffer>(buffer));
}

std::unique_ptr<RHIDescriptorSetLayout> VulkanRHI::createDescriptorSetLayout(
    RHIDescriptorSetLayoutCreateInfo& createInfo) {
  auto layoutInfo = vk::DescriptorSetLayoutCreateInfo()
                        .setBindingCount(createInfo.bindingCount)
                        .setPBindings(Cast<vk::DescriptorSetLayoutBinding>(
                            createInfo.bindings));
  vk::DescriptorSetLayout vkDescriptorSetLayout;
  if (device.createDescriptorSetLayout(&layoutInfo, nullptr,
                                       &vkDescriptorSetLayout) !=
      vk::Result::eSuccess) {
    std::cerr << "VulkanRHI::createDescriptorSetLayout  "
                 "createDescriptorSetLayout failed.\n";
    return nullptr;
  }
  auto descriptorSetLayout = std::make_unique<VulkanDescriptorSetLayout>();
  descriptorSetLayout->setResource(vkDescriptorSetLayout);
  return descriptorSetLayout;
}

void VulkanRHI::destoryDescriptorSetLayout(
    RHIDescriptorSetLayout* descriptorSetLayout) {
  device.destroyDescriptorSetLayout(
      GetResource<VulkanDescriptorSetLayout>(descriptorSetLayout));
}

std::vector<std::unique_ptr<RHIDescriptorSet>>
VulkanRHI::allocateDescriptorSets(
    const RHIDescriptorSetAllocateInfo& allocateInfo) {
  auto descriptorSets = std::vector<std::unique_ptr<RHIDescriptorSet>>(
      allocateInfo.descriptorSetCount);

  for (auto i = 0; i < allocateInfo.descriptorSetCount; i++) {
    auto descriptorSetsAllocateInfo =
        vk::DescriptorSetAllocateInfo()
            .setDescriptorPool(descriptorPool)
            .setDescriptorSetCount(1)
            .setPSetLayouts(
                Cast<vk::DescriptorSetLayout>(allocateInfo.setLayouts));
    vk::DescriptorSet vkDescriptorSet;
    if (device.allocateDescriptorSets(&descriptorSetsAllocateInfo,
                                      &vkDescriptorSet) !=
        vk::Result::eSuccess) {
      throw std::runtime_error(
          "VulkanRHI::allocateDescritorSets AllocateDescriptorSets failed.\n");
    }
    auto descriptorSet = std::make_unique<VulkanDescriptorSet>();
    descriptorSet->setResource(vkDescriptorSet);
    descriptorSets[i] = std::move(descriptorSet);
  }

  return descriptorSets;
}

void VulkanRHI::updateDescriptorSets(
    std::span<RHIWriteDescriptorSet> writeDescritorSets) {
  const auto descriptorCount = writeDescritorSets.size();
  std::vector<vk::DescriptorBufferInfo> vkDescriptorBufferInfos(
      descriptorCount);
  std::vector<vk::WriteDescriptorSet> vkWriteDescriptorSets(descriptorCount);

  for (auto i = 0; i < descriptorCount; i++) {
    const auto& bufferInfo = writeDescritorSets[i].bufferInfo;
    vkDescriptorBufferInfos[i] =
        vk::DescriptorBufferInfo()
            .setBuffer(GetResource<VulkanBuffer>(bufferInfo->buffer))
            .setOffset(bufferInfo->offset)
            .setRange(bufferInfo->range);

    const auto& descriptorSet = writeDescritorSets[i];
    vkWriteDescriptorSets[i] =
        vk::WriteDescriptorSet()
            .setDstSet(GetResource<VulkanDescriptorSet>(descriptorSet.dstSet))
            .setDstBinding(descriptorSet.dstBinding)
            .setDstArrayElement(descriptorSet.dstArrayElement)
            .setDescriptorCount(1)
            .setDescriptorType(
                Cast<vk::DescriptorType>(descriptorSet.descriptorType))
            .setPBufferInfo(&vkDescriptorBufferInfos[i])
            .setPImageInfo(nullptr)        // TODO
            .setPTexelBufferView(nullptr)  // TODO
        ;
  }
  device.updateDescriptorSets(vkWriteDescriptorSets.size(),
                              vkWriteDescriptorSets.data(), 0, nullptr);
}

uint8_t VulkanRHI::getMaxFramesInFlight() {
  return MAX_FRAMES_IN_FLIGHT;
}

uint8_t VulkanRHI::getCurrentFrameIndex() {
  return currentFrameIndex;
}

uint32_t VulkanRHI::getCurrentSwapChainImageIndex() {
  return currentSwapChainImageIndex;
}

RHISwapChainInfo VulkanRHI::getSwapChainInfo() {
  return RHISwapChainInfo{
      .extent =
          {
              .width = swapChainExtent.width,
              .height = swapChainExtent.height,
          },
      .imageFormat = static_cast<RHIFormat>(swapChainImageFormat.format),
      .imageViews =
          reinterpret_cast<RHIImageView*>(swapChainImagesViews.data()),
      .imageViewsSize = swapChainImagesViews.size(),
  };
}

RHICommandBuffer* VulkanRHI::getCurrentCommandBuffer() {
  return reinterpret_cast<VulkanCommandBuffer*>(
      &commandBuffers[currentFrameIndex]);
}

std::span<RHICommandBuffer> VulkanRHI::getCommandBuffers() {
  return {reinterpret_cast<RHICommandBuffer*>(commandBuffers.data()),
          commandBuffers.size()};
}

bool VulkanRHI::beginCommandBuffer(
    RHICommandBuffer* commandBuffer,
    RHICommandBufferBeginInfo* commandBufferBeginInfo) {
  auto beginInfo = vk::CommandBufferBeginInfo();
  if (commandBufferBeginInfo) {
    beginInfo
        .setFlags(
            Cast<vk::CommandBufferUsageFlags>(commandBufferBeginInfo->flags))
        .setPInheritanceInfo(Cast<vk::CommandBufferInheritanceInfo>(
            commandBufferBeginInfo->inheritanceInfo));
  }
  auto vkCommandBuffer = GetResource<VulkanCommandBuffer>(commandBuffer);
  if (vkCommandBuffer.begin(&beginInfo) != vk::Result::eSuccess) {
    LOG_ERROR("BeginCommandBuffer failed.")
    return false;
  }
  return true;
}

std::unique_ptr<RHICommandBuffer> VulkanRHI::beginOneTimeCommandBuffer() {
  auto allocInfo = vk::CommandBufferAllocateInfo()
                       .setLevel(vk::CommandBufferLevel::ePrimary)
                       .setCommandPool(commandPool)
                       .setCommandBufferCount(1);
  vk::CommandBuffer vkCommandBuffer;
  if (device.allocateCommandBuffers(&allocInfo, &vkCommandBuffer) !=
      vk::Result::eSuccess) {
    std::cerr << "VulkanRHI::beginOneTimeCommandBuffer allocateCommandBuffers "
                 "failed.\n";
    return nullptr;
  }
  auto beginInfo = vk::CommandBufferBeginInfo().setFlags(
      vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
  vkCommandBuffer.begin(beginInfo);

  auto commandBuffer = std::make_unique<VulkanCommandBuffer>();
  commandBuffer->setResource(vkCommandBuffer);
  return commandBuffer;
}

bool VulkanRHI::endCommandBuffer(RHICommandBuffer* commandBuffer) {
  auto vkCommandBuffer = GetResource<VulkanCommandBuffer>(commandBuffer);
  try {
    vkCommandBuffer.end();
  } catch (std::runtime_error e) {
    LOG_ERROR("EndCommandBuffer failed.")
    return false;
  }
  return true;
}

bool VulkanRHI::endOneTimeCommandBuffer(RHICommandBuffer* commandBuffer) {
  auto vkCommandBuffer = GetResource<VulkanCommandBuffer>(commandBuffer);
  vkCommandBuffer.end();

  auto submitInfo =
      vk::SubmitInfo().setCommandBufferCount(1).setPCommandBuffers(
          &vkCommandBuffer);
  if (presentQueue.submit(1, &submitInfo, nullptr) != vk::Result::eSuccess) {
    LOG_ERROR("VulkanRHI::endOneTimeCommandBuffer queueSubmit failed.\n")
    return false;
  }
  presentQueue.waitIdle();
  device.free(commandPool, 1, &vkCommandBuffer);

  return true;
}

void VulkanRHI::cmdBeginRenderPass(RHICommandBuffer* commandBuffer,
                                   RHIRenderPassBeginInfo* beginInfo,
                                   RHISubpassContents contents) {
  auto vkCommandBuffer = GetResource<VulkanCommandBuffer>(commandBuffer);

  auto renderPassBeginInfo = vk::RenderPassBeginInfo();

  if (beginInfo) {
    renderPassBeginInfo
        .setRenderPass(GetResource<VulkanRenderPass>(beginInfo->renderPass))
        .setFramebuffer(GetResource<VulkanFramebuffer>(beginInfo->frameBuffer))
        .setRenderArea(Cast<vk::Rect2D>(beginInfo->renderArea))
        .setClearValueCount(beginInfo->clearValueCount)
        .setPClearValues(Cast<vk::ClearValue>(beginInfo->clearValue));
  }

  vkCommandBuffer.beginRenderPass(renderPassBeginInfo,
                                  Cast<vk::SubpassContents>(contents));
}

void VulkanRHI::cmdEndRenderPass(RHICommandBuffer* commandBuffer) {
  auto vkCommandBuffer = GetResource<VulkanCommandBuffer>(commandBuffer);
  vkCommandBuffer.endRenderPass();
}

void VulkanRHI::cmdBindPipeline(RHICommandBuffer* commandBuffer,
                                RHIPipelineBindPoint bindPoint,
                                RHIPipeline* pipeline) {
  auto vkCommandBuffer = GetResource<VulkanCommandBuffer>(commandBuffer);
  vkCommandBuffer.bindPipeline(Cast<vk::PipelineBindPoint>(bindPoint),
                               GetResource<VulkanPipeline>(pipeline));
}

void VulkanRHI::cmdBindVertexBuffers(RHICommandBuffer* commandBuffer,
                                     uint32_t firstBinding,
                                     uint32_t bindingCount,
                                     RHIBuffer* const* buffers,
                                     const RHIDeviceSize* offsets) {
  auto vkCommandBuffer = GetResource<VulkanCommandBuffer>(commandBuffer);
  std::vector<vk::Buffer> vkBuffers;
  vkBuffers.reserve(bindingCount);
  for (uint32_t i = 0; i < bindingCount; i++) {
    vkBuffers.push_back(GetResource<VulkanBuffer>(buffers[i]));
  }
  vkCommandBuffer.bindVertexBuffers(firstBinding, bindingCount,
                                    vkBuffers.data(),
                                    Cast<vk::DeviceSize>(offsets));
}

void VulkanRHI::cmdBindIndexBuffer(RHICommandBuffer* commandBuffer,
                                   RHIBuffer* buffer,
                                   RHIDeviceSize offset,
                                   RHIIndexType indexType) {
  auto vkCommandBuffer = GetResource<VulkanCommandBuffer>(commandBuffer);
  vkCommandBuffer.bindIndexBuffer(GetResource<VulkanBuffer>(buffer), offset,
                                  Cast<vk::IndexType>(indexType));
}

void VulkanRHI::cmdBindDescriptorSets(RHICommandBuffer* commandBuffer,
                                      RHIPipelineBindPoint pipelineBindPoint,
                                      const RHIPipelineLayout* layout,
                                      uint32_t firstSet,
                                      uint32_t descriptorSetCount,
                                      const RHIDescriptorSet* descriptorSets,
                                      uint32_t dynamicOffsetCount,
                                      const uint32_t* dynamicOffsets) {
  std::vector<vk::DescriptorSet> vkDesciptorSets(descriptorSetCount);
  for (auto i = 0; i < descriptorSetCount; i++) {
    vkDesciptorSets[i] = GetResource<VulkanDescriptorSet>(descriptorSets);
  }

  auto vkCommandBuffer = GetResource<VulkanCommandBuffer>(commandBuffer);
  vkCommandBuffer.bindDescriptorSets(
      Cast<vk::PipelineBindPoint>(pipelineBindPoint),
      GetResource<VulkanPipelineLayout>(layout), firstSet, descriptorSetCount,
      vkDesciptorSets.data(), dynamicOffsetCount, dynamicOffsets);
}

void VulkanRHI::cmdDraw(RHICommandBuffer* commandBuffer,
                        uint32_t vertexCount,
                        uint32_t instanceCount,
                        uint32_t firstVertex,
                        uint32_t firstInstance) {
  auto vkCommandBuffer = GetResource<VulkanCommandBuffer>(commandBuffer);
  vkCommandBuffer.draw(vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanRHI::cmdDrawIndexed(RHICommandBuffer* commandBuffer,
                               uint32_t indexCount,
                               uint32_t instanceCount,
                               uint32_t firstIndex,
                               int32_t vertexOffset,
                               uint32_t firstInstance) {
  auto vkCommandBuffer = GetResource<VulkanCommandBuffer>(commandBuffer);
  vkCommandBuffer.drawIndexed(indexCount, instanceCount, firstIndex,
                              vertexOffset, firstInstance);
}

void VulkanRHI::cmdSetViewport(RHICommandBuffer* commandBuffer,
                               uint32_t firstViewport,
                               uint32_t viewportCount,
                               const RHIViewport* pViewports) {
  auto vkCommandBuffer = GetResource<VulkanCommandBuffer>(commandBuffer);

  vkCommandBuffer.setViewport(firstViewport, viewportCount,
                              Cast<vk::Viewport>(pViewports));
}

void VulkanRHI::cmdSetScissor(RHICommandBuffer* commandBuffer,
                              uint32_t firstScissor,
                              uint32_t scissorCount,
                              const RHIRect2D* pScissors) {
  auto vkCommandBuffer = GetResource<VulkanCommandBuffer>(commandBuffer);
  vkCommandBuffer.setScissor(firstScissor, scissorCount,
                             Cast<vk::Rect2D>(pScissors));
}

void VulkanRHI::cmdCopyBuffer(RHICommandBuffer* commandBuffer,
                              RHIBuffer* srcBuffer,
                              RHIBuffer* dstBuffer,
                              std::span<RHIBufferCopy> copyRegions) {
  auto vkCommandBuffer = GetResource<VulkanCommandBuffer>(commandBuffer);
  auto vkSrcBuffer = GetResource<VulkanBuffer>(srcBuffer);
  auto vkDstBuffer = GetResource<VulkanBuffer>(dstBuffer);
  vkCommandBuffer.copyBuffer(vkSrcBuffer, vkDstBuffer, copyRegions.size(),
                             Cast<vk::BufferCopy>(copyRegions.data()));
}

void VulkanRHI::beforePass() {
  if (device.waitForFences(1, &isFrameInFlightFences[currentFrameIndex],
                           VK_TRUE, UINT64_MAX) != vk::Result::eSuccess) {
    LOG_ERROR("WaitForFences failed.")
  }

  auto acuqireRet = device.acquireNextImageKHR(
      swapChain, UINT64_MAX,
      imageAvailableForRenderSemaphores[currentFrameIndex], VK_NULL_HANDLE,
      &currentSwapChainImageIndex);
  if (acuqireRet == vk::Result::eErrorOutOfDateKHR) {
    recreateSwapChain();
    return;
  } else if (acuqireRet != vk::Result::eSuccess &&
             acuqireRet != vk::Result::eSuboptimalKHR) {
    LOG_ERROR("AcquireNextImage failed.");
  }
  if (device.resetFences(1, &isFrameInFlightFences[currentFrameIndex]) !=
      vk::Result::eSuccess) {
    LOG_ERROR("ResetFences failed.");
  }
  commandBuffers[currentFrameIndex].reset();
}

void VulkanRHI::waitIdle() {
  device.waitIdle();
}

void VulkanRHI::submitRendering() {
  auto submitInfo =
      vk::SubmitInfo()
          .setWaitSemaphoreCount(1)
          .setPWaitSemaphores(
              &imageAvailableForRenderSemaphores[currentFrameIndex])
          .setCommandBufferCount(1)
          .setPCommandBuffers(
              Cast<vk::CommandBuffer>(&commandBuffers[currentFrameIndex]))
          .setSignalSemaphoreCount(1)
          .setPSignalSemaphores(
              &imageFinishedForPresentationSemaphores[currentFrameIndex]);

  auto presentInfo =
      vk::PresentInfoKHR()
          .setWaitSemaphoreCount(1)
          .setPWaitSemaphores(
              &imageFinishedForPresentationSemaphores[currentFrameIndex])
          .setSwapchainCount(1)
          .setPSwapchains(&swapChain)
          .setPImageIndices(&currentSwapChainImageIndex);

  if (device.resetFences(1, &isFrameInFlightFences[currentFrameIndex]) !=
      vk::Result::eSuccess) {
    LOG_ERROR("DeviceResetFences failed.")
    return;
  }

  if (presentQueue.submit(1, &submitInfo,
                          isFrameInFlightFences[currentFrameIndex]) !=
      vk::Result::eSuccess) {
    LOG_ERROR("QueueResetFences failed.")
    return;
  }

  if (presentQueue.presentKHR(presentInfo) != vk::Result::eSuccess) {
    LOG_ERROR("QueuePresentKHR failed.")
    return;
  }
  currentFrameIndex = (currentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

void* VulkanRHI::mapMemory(RHIDeviceMemory* deviceMemory,
                           RHIDeviceSize offset,
                           RHIDeviceSize size) {
  return device.mapMemory(GetResource<VulkanDeviceMemory>(deviceMemory), offset,
                          size);
}

void VulkanRHI::unmapMemory(RHIDeviceMemory* deviceMemory) {
  device.unmapMemory(GetResource<VulkanDeviceMemory>(deviceMemory));
}

void VulkanRHI::freeMemory(RHIDeviceMemory* deviceMemory) {
  device.freeMemory(GetResource<VulkanDeviceMemory>(deviceMemory));
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
      LOG_FMT("[validation layer] {}", pCallbackData->pMessage);
      break;
    case DebugSeverityFlag::eVerbose:
      break;
    case DebugSeverityFlag::eWarning:
      LOG_WARN_FMT("[validation layer] {}", pCallbackData->pMessage)
      break;
    default:
      LOG_ERROR_FMT("[validation layer] {}", pCallbackData->pMessage)
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
