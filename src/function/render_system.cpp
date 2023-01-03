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
#include "function/window_system.h"
#include "render_mesh.h"

namespace Sparrow {
void RenderSystem::initialize(const RenderSystemInitInfo& initInfo) {
  const auto rhiInitInfo = RHIInitInfo{.windowSystem = initInfo.windowSystem};
  rhi = std::make_shared<VulkanRHI>();
  rhi->initialize(rhiInitInfo);

  auto vertexCode = readFile("vert.spv");
  auto fragmentCode = readFile("frag.spv");

  auto vertexShader = rhi->createShaderModule(vertexCode);
  auto fragmentShader = rhi->createShaderModule(fragmentCode);

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

  auto swapChainInfo = rhi->getSwapChainInfo();
  auto currentFrameIndex = rhi->getCurrentFrameIndex();

  float swapChainWidth = swapChainInfo.extent.width;
  float swapChainHeight = swapChainInfo.extent.height;

  auto viewport = RHIViewport{.x = 0.0f,
                              .y = 0.0f,
                              .width = swapChainWidth,
                              .height = swapChainHeight,
                              .minDepth = 0.0f,
                              .maxDepth = 1.0f};
  auto scissor = RHIRect2D{.extend = swapChainInfo.extent};
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
      .frontFace = RHIFrontFace::Clockwise,
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

  auto attachmentDesciption = RHIAttachmentDescription{
      .format = swapChainInfo.imageFormat,
      .samples = RHISampleCount::Count1,
      .loadOp = RHIAttachmentLoadOp::Clear,
      .storeOp = RHIAttachmentStoreOp::Store,
      .stencilLoadOp = RHIAttachmentLoadOp::DontCare,
      .stencilStoreOp = RHIAttachmentStoreOp::DontCare,
      .initialLayout = RHIImageLayout::Undefined,
      .finalLayout = RHIImageLayout::PresentSrcKHR,
  };
  auto attachmentReference = RHIAttachmentReference{
      .attachment = 0, .layout = RHIImageLayout::ColorAttachmentOptimal};
  auto subpassDescription = RHISubpassDescription{
      .pipelineBindPoint = RHIPipelineBindPoint::Graphics,
      .colorAttachmentCount = 1,
      .colorAttachments = &attachmentReference,
  };
  auto renderPassCreateInfo =
      RHIRenderPassCreateInfo{.attachmentCount = 1,
                              .attachments = &attachmentDesciption,
                              .subpassCount = 1,
                              .subpasses = &subpassDescription};
  auto pipelineLayoutCreateInfo = RHIPipelineLayoutCreateInfo{
      .setLayoutCount = 0,
      .setLayouts = nullptr,
      .pushConstantRangeCount = 0,
      .pushConstantRanges = nullptr,
  };

  auto renderPass = rhi->createRenderPass(renderPassCreateInfo);
  auto piplineLayout = rhi->createPipelineLayout(pipelineLayoutCreateInfo);

  auto framebufferCreateInfo = RHIFramebufferCreateInfo{
      .renderPass = renderPass.get(),
      .attachmentCount = 1,
      .attachments = &swapChainInfo.imageViews[currentFrameIndex],
      .width = swapChainInfo.extent.width,
      .height = swapChainInfo.extent.height,
      .layers = 1,
  };

  auto framebuffer = rhi->createFramebuffer(framebufferCreateInfo);

  auto grpahicPipelineCreateInfo = RHIGraphicsPipelineCreateInfo{
      .stageCount = 2,
      .shaderStageCreateInfo = shaderStages,
      .dynamicStateCreateInfo = &dynamicStateCreateInfo,
      .vertexInputStateCreateInfo = &vertexInputStateCreateInfo,
      .inputAssemblyStateCreateInfo = &inputAssemblyCreateInfo,
      .viewportStateCreateInfo = &viewportStateCreateInfo,
      .rasterizationStateCreateInfo = &rasterizationStateCreateInfo,
      .multisampleStateCreateInfo = &multiSamplingCreateInfo,
      .depthStencilStateCreateInfo = nullptr,
      .colorBlendStateCreateInfo = &colorBlendStateCreateInfo,
      .pipelineLayout = piplineLayout.get(),
      .renderPass = renderPass.get(),
      .subpass = 0,
      .basePipelineHandle = nullptr,
      .basePipelineIndex = -1,
  };

  std::vector<Vertex> vertices = {{{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
                                  {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                  {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

  // Create vertex buffer
  auto bufferCreateInfo =
      RHIBufferCreateInfo{.size = sizeof(Vertex) * vertices.size(),
                          .usage = RHIBufferUsageFlag::TransferDst |
                                   RHIBufferUsageFlag::VertexBuffer,
                          .sharingMode = RHISharingMode::Exclusive};

  auto [vertexBuffer, deviceMemory] =
      rhi->createBuffer(bufferCreateInfo, RHIMemoryPropertyFlag::DeviceLocal);

  RHIBuffer* vertexBuffers[] = {vertexBuffer.get()};
  RHIDeviceSize offsets[] = {0};

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

  auto graphicsPipeline =
      rhi->createGraphicsPipeline(grpahicPipelineCreateInfo);

  auto commandBuffer = rhi->getCurrentCommandBuffer();

  rhi->beginCommandBuffer(commandBuffer, nullptr);

  RHIClearValue clearValue = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  auto renderPassBeginInfo =
      RHIRenderPassBeginInfo{.renderPass = renderPass.get(),
                             .frameBuffer = framebuffer.get(),
                             .renderArea =
                                 {
                                     .offset = {0, 0},
                                     .extend = swapChainInfo.extent,
                                 },
                             .clearValueCount = 1,
                             .clearValue = &clearValue};

  rhi->cmdBeginRenderPass(commandBuffer, &renderPassBeginInfo,
                          RHISubpassContents::Inline);
  rhi->cmdBindPipeline(commandBuffer, RHIPipelineBindPoint::Graphics,
                       graphicsPipeline.get());
  rhi->cmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
  rhi->cmdSetViewport(commandBuffer, 0, 1, &viewport);
  rhi->cmdSetScissor(commandBuffer, 0, 1, &scissor);
  rhi->cmdDraw(commandBuffer, 3, 1, 0, 0);
  rhi->cmdEndRenderPass(commandBuffer);
  rhi->endCommandBuffer(commandBuffer);

  rhi->submitRendering();
}
void RenderSystem::tick(float deltaTime) {}

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

}  // namespace Sparrow