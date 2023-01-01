//
// Created by arrayJY on 2022/10/14.
//

#ifndef SPARROWENGINE_VULKAN_UTILS_H
#define SPARROWENGINE_VULKAN_UTILS_H

#include <optional>
#include <vulkan/vulkan.hpp>

namespace Sparrow {

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
template <typename ResourceType, typename T>
inline typename ResourceType::Type GetResource(T* res) {
  return static_cast<ResourceType*>(res)->getResource();
}

template <typename T, typename U>
const T* Cast(U* value) {
  return reinterpret_cast<const T*>(value);
}

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

  static std::tuple<vk::Buffer, vk::DeviceMemory> createBuffer(
      vk::PhysicalDevice physicalDevice,
      vk::Device device,
      const struct RHIBufferCreateInfo& createInfo);

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
