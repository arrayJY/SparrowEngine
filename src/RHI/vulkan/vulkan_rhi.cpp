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
#include "vulkan_utils.h"

namespace Sparrow {
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
                       .setCommandBufferCount(1);
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

std::unique_ptr<RHIShader> VulkanRHI::createShaderModule(
    std::span<char> shader_code) {
  auto shaderModuleCreateInfo =
      vk::ShaderModuleCreateInfo()
          .setCodeSize(shader_code.size())
          .setPCode(reinterpret_cast<const uint32_t*>(shader_code.data()));

  vk::ShaderModule vkShaderModule;
  if (device.createShaderModule(&shaderModuleCreateInfo, nullptr,
                                &vkShaderModule) != vk::Result::eSuccess) {
    std::cerr << "CreateShaderModule failed.\n";
    return nullptr;
  }
  auto vulkanShader = std::make_unique<VulkanShader>();
  vulkanShader->setResource(vkShaderModule);
  return vulkanShader;
}

template <typename T, typename U>
  requires std::is_enum_v<T>
T Cast(U value) {
  return static_cast<T>(value);
}
template <typename T, typename U>
  requires std::is_class_v<T>
T Cast(U value) {
  return *reinterpret_cast<T*>(&value);
}

template <typename R, typename T>
R* CastResource(T* value) {
  return static_cast<R*>(value);
}

template <typename T, typename U>
const T* Cast(U* value) {
  return reinterpret_cast<const T*>(value);
}

bool VulkanRHI::createGraphicsPipeline(
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
        .setModule(
            CastResource<VulkanShader>(rhiShaderStage.module)->getResource())
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
              CastResource<VulkanPipelineLayout>(createInfo.pipelineLayout)
                  ->getResource())
          .setRenderPass(CastResource<VulkanRenderPass>(createInfo.renderPass)
                             ->getResource())
          .setSubpass(createInfo.subpass)
          .setBasePipelineHandle(
              createInfo.basePipelineHandle
                  ? CastResource<VulkanPipeline>(createInfo.basePipelineHandle)
                        ->getResource()
                  : nullptr)
          .setBasePipelineIndex(createInfo.basePipelineIndex);

  if (auto pipelineCreateResult = device.createGraphicsPipelines(
          graphicsPipelineCache, graphicsPipelineCreateInfo);
      pipelineCreateResult.result == vk::Result::eSuccess) {
    graphicsPipeline = pipelineCreateResult.value;
    return true;
  }

  return false;
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

RHISwapChainInfo VulkanRHI::getSwapChainInfo() {
  return RHISwapChainInfo{
      .extent =
          {
              .width = swapChainExtent.width,
              .height = swapChainExtent.height,
          },
      .imageFormat = static_cast<RHIFormat>(swapChainImageFormat.format)};
}

void VulkanRHI::submitRendering() {
  auto submitInfo =
      vk::SubmitInfo()
          .setWaitSemaphoreCount(1)
          .setPWaitSemaphores(
              &imageAvailableForRenderSemaphores[currentSwapChainImageIndex])
          .setCommandBufferCount(1)
          .setPCommandBuffers(&commandBuffers[currentFrameIndex])
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
    std::cerr << "DeviceResetFences failed.";
    return;
  }

  if (presentQueue.submit(1, &submitInfo,
                          isFrameInFlightFences[currentFrameIndex]) !=
      vk::Result::eSuccess) {
    std::cerr << "QueueResetFences failed.";
    return;
  }

  if (presentQueue.presentKHR(presentInfo) != vk::Result::eSuccess) {
    std::cerr << "QueuePresentKHR failed.";
    return;
  }

  currentFrameIndex = (currentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
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
