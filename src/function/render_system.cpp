//
// Created by arrayJY on 2022/12/6.
//

#include "render_system.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include "RHI/vulkan/vulkan_rhi.h"
#include "function/window_system.h"

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

  auto vertexInputStateCreateInfo = RHIVertexInputStateCreateInfo{
      .vertexBindingDescriptionCount = 0,
      .vertexBindingDescriptions = nullptr,
      .vertexAttributeDescriptionCount = 0,
      .vertexAttributeDescriptions = nullptr,
  };

  auto inputAssemblyCreateInfo = RHIInputAssemblyStateCreateInfo{
      .topology = RHIPrimitiveTopology::TriangleList,
      .primitiveRestartEnabled = RHIFalse,
  };

  auto swapChainInfo = rhi->getSwapChainInfo();

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

  rhi->createGraphicsPipeline(grpahicPipelineCreateInfo);
}
void RenderSystem::tick(float deltaTime) {}

std::vector<char> RenderSystem::readFile(const std::string& filename) {
  // std::filesystem::relative(const path &Path)
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