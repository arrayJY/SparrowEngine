//
// Created by arrayJY on 2023/01/18.
//

#ifndef SPARROWENGINE_RENDER_RESOURCE_H
#define SPARROWENGINE_RENDER_RESOURCE_H

#include <RHI/rhi_struct.h>
#include <string>


namespace Sparrow {
class RenderResource {
 public:
  virtual void load(const std::string& path) = 0;
  virtual std::byte* getData();
  virtual ~RenderResource() = 0;

 protected:
  std::byte* data;
};

class RenderTexture : public RenderResource {
 public:
  void load(const std::string& path) override;
  ~RenderTexture();

  int width = 0;
  int height = 0;
  int channels = 0;
  RHIDeviceSize imageSize = 0;
};

}  // namespace Sparrow

#endif