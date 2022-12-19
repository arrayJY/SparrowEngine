//
// Created by arrayJY on 2022/10/14.
//

#ifndef SPARROWENGINE_VULKAN_UTILS_H
#define SPARROWENGINE_VULKAN_UTILS_H

#include <optional>
#include <vulkan/vulkan.hpp>

namespace Sparrow {

class VulkanUtils {
 public:
  static void createImage(vk::PhysicalDevice physicalDevice,
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
                          vk::DeviceMemory& deviceMemory);

  static vk::ImageView createImageView(vk::Device device,
                                       vk::Image& image,
                                       vk::Format format,
                                       vk::ImageAspectFlags aspectFlags,
                                       vk::ImageViewType viewType,
                                       uint32_t layoutCount,
                                       uint32_t mipLevels);

  static uint32_t findMemoryType(vk::PhysicalDevice physicalDevice,
                                 uint32_t typeFilter,
                                 vk::MemoryPropertyFlags memoryPropertyFlags);

  template <typename T>
  inline auto scopedEnumValue(T v) {
    return static_cast<typename std::underlying_type<T>::type>(v);
  }
};

}  // namespace Sparrow

#endif  // SPARROWENGINE_VULKAN_UTILS_H
