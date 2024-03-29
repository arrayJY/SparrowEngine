//
// Created by arrayJY on 2022/12/6.
//

#include "render_system.h"
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "RHI/vulkan/vulkan_rhi.h"
#include "RHI/vulkan/vulkan_rhi_resource.h"
#include "RHI/vulkan/vulkan_utils.h"
#include "function/render_resource.h"
#include "function/window_system.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Sparrow {
void RenderSystem::initialize(const RenderSystemInitInfo& initInfo) {
  const auto rhiInitInfo = RHIInitInfo{.windowSystem = initInfo.windowSystem};
  rhi = std::make_shared<VulkanRHI>();
  rhi->initialize(rhiInitInfo);

  auto vertexCode = readFile("shader.vert.spv");
  auto fragmentCode = readFile("shader.frag.spv");

  vertexShader = rhi->createShaderModule(vertexCode);
  fragmentShader = rhi->createShaderModule(fragmentCode);

  RHIPipelineShaderStageCreateInfo vertexPipelineShaderStageCreateInfo = {
      .stage = RHIShaderStageFlag::Vertex,
      .module = vertexShader.get(),
      .name = "main"};

  RHIPipelineShaderStageCreateInfo fragmentPipelineShaderStageCreateInfo = {
      .stage = RHIShaderStageFlag::Fragment,
      .module = fragmentShader.get(),
      .name = "main"};
  RHIPipelineShaderStageCreateInfo shaderStages[] = {
      vertexPipelineShaderStageCreateInfo,
      fragmentPipelineShaderStageCreateInfo};

  auto bindingDescription = Vertex::getBindingDescription();
  auto attributeDescriptions = Vertex::getAttributeDescription();
  auto vertexInputStateCreateInfo = RHIVertexInputStateCreateInfo{
      .vertexBindingDescriptionCount = 1,
      .vertexBindingDescriptions = &bindingDescription,
      .vertexAttributeDescriptionCount = attributeDescriptions.size(),
      .vertexAttributeDescriptions = attributeDescriptions.data(),
  };

  auto inputAssemblyCreateInfo = RHIInputAssemblyStateCreateInfo{
      .topology = RHIPrimitiveTopology::TriangleList,
      .primitiveRestartEnabled = RHIFalse,
  };

  const auto swapChainInfo = rhi->getSwapChainInfo();
  const auto maxFrameInFlight = rhi->getMaxFramesInFlight();

  float swapChainWidth = swapChainInfo.extent.width;
  float swapChainHeight = swapChainInfo.extent.height;

  viewport = RHIViewport{.x = 0.0f,
                         .y = 0.0f,
                         .width = swapChainWidth,
                         .height = swapChainHeight,
                         .minDepth = 0.0f,
                         .maxDepth = 1.0f};
  scissor = RHIRect2D{.extend = swapChainInfo.extent};
  auto viewportStateCreateInfo =
      RHIViewportStateCreateInfo{.viewportCount = 1,
                                 .viewports = &viewport,
                                 .scissorCount = 1,
                                 .scissors = &scissor};

  RHIDynamicState dynamicStates[] = {RHIDynamicState::Viewport,
                                     RHIDynamicState::Scissor};
  auto dynamicStateCreateInfo = RHIDynamicStateCreateInfo{
      .dynamicStateCount = 2,
      .dynamicStates = dynamicStates,
  };

  auto rasterizationStateCreateInfo = RHIRasterizationStateCreateInfo{
      .rasterizerDiscardEnable = RHIFalse,
      .polygonMode = RHIPolygonMode::Fill,
      .cullMode = RHICullMode::Back,
      .frontFace = RHIFrontFace::CounterClockwise,
      .depthBiasEnable = RHIFalse,
      .depthBiasConstantFactor = 0.0f,
      .depthBiasClamp = 0.0f,
      .depthBiasSlopeFactor = 0.0f,
      .lineWidth = 1.0f,
  };

  auto multiSamplingCreateInfo = RHIMultisampleStateCreateInfo{
      .rasterizationSamples = RHISampleCount::Count1,
      .sampleShadingEnable = RHIFalse,
      .minSampleShading = 1.0f,
      .sampleMask = nullptr,
      .alphaToCoverageEnable = RHIFalse,
      .alphaToOneEnable = RHIFalse,
  };

  auto colorBlendAttachment = RHIColorBlendAttachmentState{
      .blendEnable = RHIFalse,
      .srcColorBlendFactor = RHIBlendFactor::One,
      .dstColorBlendFactor = RHIBlendFactor::Zero,
      .colorBlendOp = RHIBlendOp::Add,
      .srcAlphaBlendFactor = RHIBlendFactor::One,
      .dstAlphaBlendFactor = RHIBlendFactor::Zero,
      .alphaBlendOp = RHIBlendOp::Add,
      .colorWriteMask = RHIColorComponentFlag::AllBits,
  };

  auto colorBlendStateCreateInfo =
      RHIColorBlendStateCreateInfo{.logicOpEnable = RHIFalse,
                                   .logicOp = RHILogicOp::Copy,
                                   .attachmentCount = 1,
                                   .attachments = &colorBlendAttachment,
                                   .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}};

  auto colorAttachmentDesciption = RHIAttachmentDescription{
      .format = swapChainInfo.imageFormat,
      .samples = RHISampleCount::Count1,
      .loadOp = RHIAttachmentLoadOp::Clear,
      .storeOp = RHIAttachmentStoreOp::Store,
      .stencilLoadOp = RHIAttachmentLoadOp::DontCare,
      .stencilStoreOp = RHIAttachmentStoreOp::DontCare,
      .initialLayout = RHIImageLayout::Undefined,
      .finalLayout = RHIImageLayout::PresentSrcKHR,
  };
  auto colorAttachmentReference = RHIAttachmentReference{
      .attachment = 0, .layout = RHIImageLayout::ColorAttachmentOptimal};

  auto depthImageInfo = rhi->getDepthImageInfo();
  auto depthAttachmentDescription = RHIAttachmentDescription{
      .format = depthImageInfo.format,
      .samples = RHISampleCount::Count1,
      .loadOp = RHIAttachmentLoadOp::Clear,
      .storeOp = RHIAttachmentStoreOp::DontCare,
      .stencilLoadOp = RHIAttachmentLoadOp::DontCare,
      .stencilStoreOp = RHIAttachmentStoreOp::DontCare,
      .initialLayout = RHIImageLayout::Undefined,
      .finalLayout = RHIImageLayout::DepthStencilAttachmentOptimal,
  };
  auto depthAttachmentReference = RHIAttachmentReference{
      .attachment = 1,
      .layout = RHIImageLayout::DepthStencilAttachmentOptimal,
  };

  auto subpassDescription = RHISubpassDescription{
      .pipelineBindPoint = RHIPipelineBindPoint::Graphics,
      .colorAttachmentCount = 1,
      .colorAttachments = &colorAttachmentReference,
      .depthStencilAttachment = &depthAttachmentReference,
  };
  auto subpassDependency = RHISubpassDependency{
      .srcSubpass = RHISubpassExternal,
      .dstSubpass = 0,
      .srcStageMask = RHIPipelineStageFlag::ColorAttachmentOutput |
                      RHIPipelineStageFlag::EarlyFragmentTests,
      .dstStageMask = RHIPipelineStageFlag::ColorAttachmentOutput |
                      RHIPipelineStageFlag::EarlyFragmentTests,
      .srcAccessMask = RHIAccessFlag::None,
      .dstAccessMask = RHIAccessFlag::ColorAttachmentWrite |
                       RHIAccessFlag::DepthStencilAttachmentWrite,
  };

  std::array<RHIAttachmentDescription, 2> renderPassAttachments = {
      colorAttachmentDesciption, depthAttachmentDescription};

  auto renderPassCreateInfo =
      RHIRenderPassCreateInfo{.attachmentCount = renderPassAttachments.size(),
                              .attachments = renderPassAttachments.data(),
                              .subpassCount = 1,
                              .subpasses = &subpassDescription,
                              .dependencyCount = 1,
                              .dependencies = &subpassDependency};

  auto depthStencilCreateInfo = RHIDepthStencilStateCreateInfo{
    .depthTestEnable = RHITrue,
    .depthWriteEnable = RHITrue,
    .depthCompareOp = RHICompareOp::Less,
    .depthBoundsTestEnable = RHIFalse,
    .stencilTestEnable = RHIFalse,
    .front = {},
    .back = {},
    .minDepthBounds = 0.0f,
    .maxDepthBounds = 1.0f,
  };

  auto uboLayoutBinding = RHIDescriptorSetLayoutBinding{
      .binding = 0,
      .descriptorType = RHIDescriptorType::UniformBuffer,
      .descriptorCount = 1,
      .stageFlags = RHIShaderStageFlag::Vertex,
      .immutableSamplers = nullptr,
  };
  auto sampleLayoutBinding = RHIDescriptorSetLayoutBinding{
      .binding = 1,
      .descriptorType = RHIDescriptorType::CombinedImageSampler,
      .descriptorCount = 1,
      .stageFlags = RHIShaderStageFlag::Fragment,
      .immutableSamplers = nullptr,
  };
  std::array<RHIDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding,
                                                           sampleLayoutBinding};

  auto descriptorSetLayoutCreateInfo = RHIDescriptorSetLayoutCreateInfo{
      .bindingCount = bindings.size(),
      .bindings = bindings.data(),
  };

  descriptorSetLayout =
      rhi->createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
  descriptorSets = rhi->allocateDescriptorSets(RHIDescriptorSetAllocateInfo{
      .descriptorPool = RHIDescriptorPool{},  // TODO: use outer resource
      .descriptorSetCount = maxFrameInFlight,
      .setLayouts = descriptorSetLayout.get(),
  });

  auto pipelineLayoutCreateInfo = RHIPipelineLayoutCreateInfo{
      .setLayoutCount = 1,
      .setLayouts = descriptorSetLayout.get(),
      .pushConstantRangeCount = 0,
      .pushConstantRanges = nullptr,
  };

  renderPass = rhi->createRenderPass(renderPassCreateInfo);
  piplineLayout = rhi->createPipelineLayout(pipelineLayoutCreateInfo);

  framebuffers.reserve(swapChainInfo.imageViewsSize);
  for (auto i = 0U; i < swapChainInfo.imageViewsSize; i++) {
    std::array<RHIImageView*, 2> attachments = {rhi->getSwapChainImageView(i),
                                                depthImageInfo.imageView};
    auto framebufferCreateInfo = RHIFramebufferCreateInfo{
        .renderPass = renderPass.get(),
        .attachmentCount = attachments.size(),
        .attachments = attachments.data(),
        .width = swapChainInfo.extent.width,
        .height = swapChainInfo.extent.height,
        .layers = 1,
    };
    framebuffers.push_back(rhi->createFramebuffer(framebufferCreateInfo));
  }

  auto grpahicPipelineCreateInfo = RHIGraphicsPipelineCreateInfo{
      .stageCount = 2,
      .shaderStageCreateInfo = shaderStages,
      .dynamicStateCreateInfo = &dynamicStateCreateInfo,
      .vertexInputStateCreateInfo = &vertexInputStateCreateInfo,
      .inputAssemblyStateCreateInfo = &inputAssemblyCreateInfo,
      .viewportStateCreateInfo = &viewportStateCreateInfo,
      .rasterizationStateCreateInfo = &rasterizationStateCreateInfo,
      .multisampleStateCreateInfo = &multiSamplingCreateInfo,
      .depthStencilStateCreateInfo = &depthStencilCreateInfo,
      .colorBlendStateCreateInfo = &colorBlendStateCreateInfo,
      .pipelineLayout = piplineLayout.get(),
      .renderPass = renderPass.get(),
      .subpass = 0,
      .basePipelineHandle = nullptr,
      .basePipelineIndex = -1,
  };

  vertices = {{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
              {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
              {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
              {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

              {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
              {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
              {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
              {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};

  indices = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};

  auto [_vertexBuffer, _vertexBufferMemory] = createVertexBuffer(vertices);
  auto [_indexBuffer, _indexBufferMemory] = createIndexBuffer(indices);
  auto [_uniformBuffers, _uniformBufferMemories, _uniformBufferMappedMemories] =
      createUniformBuffers();
  auto [_textureImage, _textureImageView, _textureImageMemory] =
      createTextureImage();

  vertexBuffer = std::move(_vertexBuffer);
  vertexBufferMemory = std::move(_vertexBufferMemory);
  indexBuffer = std::move(_indexBuffer);
  indexBufferMemory = std::move(_indexBufferMemory);
  uniformBuffers = std::move(_uniformBuffers);
  uniformBufferMemories = std::move(_uniformBufferMemories);
  uniformBuffersMappedMemories = std::move(_uniformBufferMappedMemories);
  textureImage = std::move(_textureImage);
  textureImageView = std::move(_textureImageView);
  textureImageMemory = std::move(_textureImageMemory);

  textureSampler = rhi->createSampler(RHISamplerCreateInfo{
      .magFilter = RHIFilter::Linear,
      .minFilter = RHIFilter::Linear,
      .mipmapMode = RHISamplerMipmapMode::Linear,
      .addressModeU = RHISamplerAddressMode::Repeat,
      .addressModeV = RHISamplerAddressMode::Repeat,
      .addressModeW = RHISamplerAddressMode::Repeat,
      .mipLodBias = 0.0f,
      .anisotropyEnable = RHITrue,
      .maxAnisotropy = 0,
      .compareEnable = RHIFalse,
      .compareOp = RHICompareOp::Always,
      .minLod = 0.0f,
      .maxLod = 0.0f,
      .borderColor = RHIBorderColor::IntOpaqueBlack,
      .unnormalizedCoordinates = RHIFalse,
  });

  std::vector<RHIDescriptorBufferInfo> bufferInfos;
  std::vector<RHIDescriptorImageInfo> imageInfos;
  std::vector<RHIWriteDescriptorSet> writeDescriptorSets;
  std::vector<RHIWriteDescriptorSet> samplerWriteDescriptorSets;
  bufferInfos.reserve(maxFrameInFlight);
  imageInfos.reserve(maxFrameInFlight);
  writeDescriptorSets.reserve(maxFrameInFlight);
  samplerWriteDescriptorSets.reserve(maxFrameInFlight);

  for (auto i = 0; i < maxFrameInFlight; i++) {
    bufferInfos.push_back(RHIDescriptorBufferInfo{
        .buffer = uniformBuffers[i].get(),
        .offset = 0,
        .range = sizeof(Transform),
    });

    imageInfos.push_back(RHIDescriptorImageInfo{
        .sampler = textureSampler.get(),
        .imageView = textureImageView.get(),
        .imageLayout = RHIImageLayout::ReadOnlyOptimal,
    });

    writeDescriptorSets.push_back(RHIWriteDescriptorSet{
        .dstSet = descriptorSets[i].get(),
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = RHIDescriptorType::UniformBuffer,
        .imageInfo = nullptr,
        .bufferInfo = &bufferInfos[i],
        .texelBufferView = nullptr,
    });

    samplerWriteDescriptorSets.push_back(RHIWriteDescriptorSet{
        .dstSet = descriptorSets[i].get(),
        .dstBinding = 1,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = RHIDescriptorType::CombinedImageSampler,
        .imageInfo = &imageInfos[i],
        .bufferInfo = nullptr,
        .texelBufferView = nullptr,
    });
  }
  rhi->updateDescriptorSets(writeDescriptorSets);
  rhi->updateDescriptorSets(samplerWriteDescriptorSets);

  graphicsPipeline = rhi->createGraphicsPipeline(grpahicPipelineCreateInfo);
}

void RenderSystem::tick(float deltaTime) {
  rhi->beforePass();
  updateUniformBuffer(
      uniformBuffersMappedMemories[rhi->getCurrentFrameIndex()]);
  auto commandBuffer = rhi->getCurrentCommandBuffer();
  recordCommandBuffer(commandBuffer);
  rhi->submitRendering();
  rhi->waitIdle();
}

std::vector<char> RenderSystem::readFile(const std::string& filename) {
  char const* shader_dir = SHADER_DIR;
  auto path = std::filesystem::path(shader_dir);
  path /= filename;

  std::ifstream file(path, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }

  size_t fileSize = file.tellg();
  std::vector<char> buffer(fileSize);
  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();
  return buffer;
}

std::tuple<std::unique_ptr<RHIBuffer>, std::unique_ptr<RHIDeviceMemory>>
RenderSystem::createIndexBuffer(std::span<uint16_t> indices) {
  auto bufferSize = sizeof(indices[0]) * indices.size();

  auto stagingBufferCreateInfo = RHIBufferCreateInfo{
      .size = bufferSize,
      .usage = RHIBufferUsageFlag::TransferSrc,
      .sharingMode = RHISharingMode::Exclusive,
  };
  auto [stagingBuffer, stagingBufferMemory] = rhi->createBuffer(
      stagingBufferCreateInfo,
      RHIMemoryPropertyFlag::HostVisible | RHIMemoryPropertyFlag::HostCoherent);
  auto stagingBufferMappedMemory = rhi->mapMemory(stagingBufferMemory.get(), 0,
                                                  stagingBufferCreateInfo.size);
  std::memcpy(stagingBufferMappedMemory, indices.data(),
              stagingBufferCreateInfo.size);
  rhi->unmapMemory(stagingBufferMemory.get());

  auto indexBufferCreateInfo =
      RHIBufferCreateInfo{.size = bufferSize,
                          .usage = RHIBufferUsageFlag::TransferDst |
                                   RHIBufferUsageFlag::IndexBuffer,
                          .sharingMode = RHISharingMode::Exclusive};

  auto [indexBuffer, indexBufferMemory] = rhi->createBuffer(
      indexBufferCreateInfo, RHIMemoryPropertyFlag::DeviceLocal);

  auto copyRegion =
      RHIBufferCopy{.srcOffset = 0, .dstOffset = 0, .size = bufferSize};

  auto oneTimeCommandBuffer = rhi->beginOneTimeCommandBuffer();
  rhi->cmdCopyBuffer(oneTimeCommandBuffer.get(), stagingBuffer.get(),
                     indexBuffer.get(), {&copyRegion, 1});
  rhi->endOneTimeCommandBuffer(oneTimeCommandBuffer.get());
  rhi->destoryBuffer(stagingBuffer.get());
  rhi->freeMemory(stagingBufferMemory.get());
  return std::make_tuple(std::move(indexBuffer), std::move(indexBufferMemory));
}

std::tuple<std::unique_ptr<RHIBuffer>, std::unique_ptr<RHIDeviceMemory>>
RenderSystem::createVertexBuffer(std::span<Vertex> vertices) {
  // Create vertex buffer
  auto bufferCreateInfo =
      RHIBufferCreateInfo{.size = sizeof(Vertex) * vertices.size(),
                          .usage = RHIBufferUsageFlag::TransferDst |
                                   RHIBufferUsageFlag::VertexBuffer,
                          .sharingMode = RHISharingMode::Exclusive};

  auto [vertexBuffer, vertexBufferMemory] =
      rhi->createBuffer(bufferCreateInfo, RHIMemoryPropertyFlag::DeviceLocal);

  // Create Staging buffer
  auto stagingBufferCreateInfo = RHIBufferCreateInfo{
      .size = sizeof(Vertex) * vertices.size(),
      .usage = RHIBufferUsageFlag::TransferSrc,
      .sharingMode = RHISharingMode::Exclusive,
  };
  auto [stagingBuffer, stagingBufferMemory] = rhi->createBuffer(
      stagingBufferCreateInfo,
      RHIMemoryPropertyFlag::HostVisible | RHIMemoryPropertyFlag::HostCoherent);
  auto stagingBufferMappedMemory = rhi->mapMemory(stagingBufferMemory.get(), 0,
                                                  stagingBufferCreateInfo.size);
  std::memcpy(stagingBufferMappedMemory, vertices.data(),
              stagingBufferCreateInfo.size);
  rhi->unmapMemory(stagingBufferMemory.get());

  auto copyRegion = RHIBufferCopy{
      .srcOffset = 0, .dstOffset = 0, .size = stagingBufferCreateInfo.size};

  auto oneTimeCommandBuffer = rhi->beginOneTimeCommandBuffer();
  rhi->cmdCopyBuffer(oneTimeCommandBuffer.get(), stagingBuffer.get(),
                     vertexBuffer.get(), {&copyRegion, 1});
  rhi->endOneTimeCommandBuffer(oneTimeCommandBuffer.get());
  rhi->destoryBuffer(stagingBuffer.get());
  rhi->freeMemory(stagingBufferMemory.get());
  return std::make_tuple(std::move(vertexBuffer),
                         std::move(vertexBufferMemory));
}

std::tuple<std::vector<std::unique_ptr<RHIBuffer>>,
           std::vector<std::unique_ptr<RHIDeviceMemory>>,
           std::vector<void*>>
RenderSystem::createUniformBuffers() {
  std::vector<std::unique_ptr<RHIBuffer>> uniformBuffers;
  std::vector<std::unique_ptr<RHIDeviceMemory>> uniformBufferMemorys;
  std::vector<void*> uniformBuffersMapped;

  RHIDeviceSize bufferSize = sizeof(Transform);
  const auto maxFramesInFlight = rhi->getMaxFramesInFlight();

  uniformBuffers.resize(maxFramesInFlight);
  uniformBufferMemorys.resize(maxFramesInFlight);
  uniformBuffersMapped.resize(maxFramesInFlight);

  auto bufferCreateInfo = RHIBufferCreateInfo{
      .size = bufferSize,
      .usage = RHIBufferUsageFlag::UniformBuffer,
      .sharingMode = RHISharingMode::Exclusive,
  };
  for (auto i = 0; i < maxFramesInFlight; i++) {
    auto [buffer, deviceMemory] = rhi->createBuffer(
        bufferCreateInfo, RHIMemoryPropertyFlag::HostVisible |
                              RHIMemoryPropertyFlag::HostCoherent);
    uniformBuffersMapped[i] = rhi->mapMemory(deviceMemory.get(), 0, bufferSize);
    uniformBuffers[i] = std::move(buffer);
    uniformBufferMemorys[i] = std::move(deviceMemory);
  }

  return std::make_tuple(std::move(uniformBuffers),
                         std::move(uniformBufferMemorys),
                         std::move(uniformBuffersMapped));
}

std::tuple<std::unique_ptr<RHIImage>,
           std::unique_ptr<RHIImageView>,
           std::unique_ptr<RHIDeviceMemory>>
RenderSystem::createTextureImage() {
  RenderTexture texture;
  texture.load(TEST_TEXTURE_PATH);

  auto [image, imageView, imageMemory] = rhi->createImageAndCopyData(
      RHIImageCreateInfo{
          .width = static_cast<uint32_t>(texture.width),
          .height = static_cast<uint32_t>(texture.height),
          .format = RHIFormat::R8G8B8A8Srgb,
          .tiling = RHIImageTiling::Optimal,
          .imageUsageFlags =
              RHIImageUsageFlag::TransferDst | RHIImageUsageFlag::Sampled,
          .memoryPropertyFlags = RHIMemoryPropertyFlag::DeviceLocal,
          .arrayLayers = 1,
          .mipLevels = 1,
      },
      texture.getData(), texture.imageSize);

  return std::make_tuple(std::move(image), std::move(imageView),
                         std::move(imageMemory));
}

void RenderSystem::updateUniformBuffer(void* mappedMemory) {
  static auto startTime = std::chrono::high_resolution_clock::now();
  auto currentTime = std::chrono::high_resolution_clock::now();
  auto swapChainInfo = rhi->getSwapChainInfo();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(
                   currentTime - startTime)
                   .count();
  auto ubo = Transform{
      .model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f),
                           glm::vec3(0.0f, 0.0f, 1.0f)),
      .view =
          glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                      glm::vec3(0.0f, 0.0f, 1.0f)),
      .projection = glm::perspective(
          glm::radians(45.0f),
          swapChainInfo.extent.width / (float)swapChainInfo.extent.height, 0.1f,
          10.0f),
  };
  ubo.projection[1][1] *= -1;
  std::memcpy(mappedMemory, &ubo, sizeof(ubo));
};

void RenderSystem::recordCommandBuffer(RHICommandBuffer* commandBuffer) {
  auto swapChainInfo = rhi->getSwapChainInfo();
  auto imageIndex = rhi->getCurrentSwapChainImageIndex();
  rhi->beginCommandBuffer(commandBuffer, nullptr);

  std::array<RHIClearValue, 2> clearValues = {
      RHIClearValue{.color = {{0.0f, 0.0f, 0.0f, 1.0f}}},
      RHIClearValue{.depthStencil = {1.0f, 0}},
  };
  auto renderPassBeginInfo =
      RHIRenderPassBeginInfo{.renderPass = renderPass.get(),
                             .frameBuffer = framebuffers[imageIndex].get(),
                             .renderArea =
                                 {
                                     .offset = {0, 0},
                                     .extend = swapChainInfo.extent,
                                 },
                             .clearValueCount = clearValues.size(),
                             .clearValue = clearValues.data()};
  rhi->cmdBeginRenderPass(commandBuffer, &renderPassBeginInfo,
                          RHISubpassContents::Inline);
  rhi->cmdBindPipeline(commandBuffer, RHIPipelineBindPoint::Graphics,
                       graphicsPipeline.get());

  RHIBuffer* vertexBuffers[] = {vertexBuffer.get()};
  RHIDeviceSize offsets[] = {0};
  rhi->cmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
  rhi->cmdBindIndexBuffer(commandBuffer, indexBuffer.get(), 0,
                          RHIIndexType::Uint16);
  rhi->cmdBindDescriptorSets(commandBuffer, RHIPipelineBindPoint::Graphics,
                             piplineLayout.get(), 0, 1,
                             descriptorSets[imageIndex].get(), 0, nullptr);
  rhi->cmdSetViewport(commandBuffer, 0, 1, &viewport);
  rhi->cmdSetScissor(commandBuffer, 0, 1, &scissor);
  //   rhi->cmdDraw(commandBuffer, 3, 1, 0, 0);
  rhi->cmdDrawIndexed(commandBuffer, indices.size(), 1, 0, 0, 0);
  rhi->cmdEndRenderPass(commandBuffer);
  rhi->endCommandBuffer(commandBuffer);
}

}  // namespace Sparrow