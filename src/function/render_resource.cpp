#include "render_resource.h"
#include <iostream>
#include "utils/log.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
namespace Sparrow {
void RenderTexture::load(const std::string& path) {
  stbi_uc* image =
      stbi_load(path.data(), &width, &height, &channels, STBI_rgb_alpha);
  data = reinterpret_cast<std::byte*>(image);
  imageSize = width * height * 4;
  if (!data) {
    LOG_ERROR("Load RenderTexture failed.");
  }
}

RenderTexture::~RenderTexture() {
  stbi_image_free(data);
}

std::byte* RenderResource::getData() {
  return data;
}

}  // namespace Sparrow