//
// Created by arrayJY on 2022/10/14.
//

#include "vulkan_utils.h"
#include <iostream>
#include "RHI/rhi_struct.h"

namespace Sparrow {
void VulkanUtils::createImage(
    vk::PhysicalDevice physicalDevice,
    vk::Device device,
    uint32_t width,
    uint32_t height,
    vk::Format format,
    vk::ImageTiling tiling,
    vk::ImageUsageFlags imageUsageFlags,
    vk::MemoryPropertyFlags memoryPropertyFlags,
    std::optional<vk::ImageCreateFlags> imageCreateFlags,
    uint32_t arrayLayers,
    uint32_t mipLevels,
    vk::Image& image,
    vk::DeviceMemory& deviceMemory) {
  auto imageCreateInfo = vk::ImageCreateInfo()
                             .setImageType(vk::ImageType::e2D)
                             .setExtent(vk::Extent3D(width, height, 1))
                             .setMipLevels(mipLevels)
                             .setArrayLayers(arrayLayers)
                             .setFormat(format)
                             .setTiling(tiling)
                             .setInitialLayout(vk::ImageLayout::eUndefined)
                             .setSamples(vk::SampleCountFlagBits::e1)
                             .setSharingMode(vk::SharingMode::eExclusive)
                             .setUsage(imageUsageFlags);
  imageCreateInfo = imageCreateFlags.has_value()
                        ? imageCreateInfo.setFlags(imageCreateFlags.value())
                        : imageCreateInfo;
  image = device.createImage(imageCreateInfo);

  auto memRequirements = device.getImageMemoryRequirements(image);
  auto allocInfo = vk::MemoryAllocateInfo()
                       .setAllocationSize(memRequirements.size)
                       .setMemoryTypeIndex(findMemoryType(
                           physicalDevice, memRequirements.memoryTypeBits,
                           memoryPropertyFlags));

  deviceMemory = device.allocateMemory(allocInfo);
  device.bindImageMemory(image, deviceMemory, 0);
}

uint32_t VulkanUtils::findMemoryType(
    vk::PhysicalDevice physicalDevice,
    uint32_t typeFilter,
    vk::MemoryPropertyFlags memoryPropertyFlags) {
  auto memoryProperties = physicalDevice.getMemoryProperties();

  for (auto i = 0; i < memoryProperties.memoryTypeCount; i++) {
    if (typeFilter & (1 << i) &&
        (memoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) ==
            memoryPropertyFlags) {
      return i;
    }
  }
  throw std::runtime_error("VulkanUtils::findMemoryType");
}

vk::ImageView VulkanUtils::createImageView(vk::Device device,
                                           vk::Image& image,
                                           vk::Format format,
                                           vk::ImageAspectFlags aspectFlags,
                                           vk::ImageViewType viewType,
                                           uint32_t layoutCount,
                                           uint32_t mipLevels) {
  auto createInfo = vk::ImageViewCreateInfo()
                        .setImage(image)
                        .setViewType(viewType)
                        .setFormat(format)
                        .setSubresourceRange(vk::ImageSubresourceRange(
                            aspectFlags, 0, mipLevels, 0, layoutCount));
  auto imageView = device.createImageView(createInfo);
  return imageView;
}

std::tuple<vk::Buffer, vk::DeviceMemory> VulkanUtils::createBuffer(
    vk::PhysicalDevice physicalDevice,
    vk::Device device,
    const struct RHIBufferCreateInfo& createInfo,
    RHIMemoryPropertyFlag properties) {
  auto bufferCreateInfo =
      vk::BufferCreateInfo()
          .setSize(createInfo.size)
          .setUsage(Cast<vk::BufferUsageFlags>(createInfo.usage))
          .setSharingMode(Cast<vk::SharingMode>(createInfo.sharingMode));
  vk::Buffer buffer;
  vk::DeviceMemory deviceMemory;
  if (device.createBuffer(&bufferCreateInfo, nullptr, &buffer) !=
      vk::Result::eSuccess) {
    std::cerr << "CreateBuffer failed.";
    return std::make_tuple(buffer, deviceMemory);
  }
  auto memRequirements = device.getBufferMemoryRequirements(buffer);

  auto allocInfo = vk::MemoryAllocateInfo()
                       .setAllocationSize(memRequirements.size)
                       .setMemoryTypeIndex(findMemoryType(
                           physicalDevice, memRequirements.memoryTypeBits,
                           Cast<vk::MemoryPropertyFlags>(properties)));
  if (device.allocateMemory(&allocInfo, nullptr, &deviceMemory) !=
      vk::Result::eSuccess) {
    std::cerr << "AllocateMemory failed.";
    return std::make_tuple(buffer, deviceMemory);
  }
  device.bindBufferMemory(buffer, deviceMemory, 0);
  return std::make_tuple(buffer, deviceMemory);
}

}  // namespace Sparrow