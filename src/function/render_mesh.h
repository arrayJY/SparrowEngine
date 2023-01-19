//
// Created by arrayJY on 2023/1/2.
//

#ifndef SPARROWENGINE_RENDER_MESH_H
#define SPARROWENGINE_RENDER_MESH_H

#include <array>
#include "RHI/rhi_struct.h"
#include "glm/glm.hpp"

namespace Sparrow {

struct Vertex {
  glm::vec2 position;
  glm::vec3 color;
  glm::vec2 texCoord;

  static RHIVertexBindingDescription getBindingDescription() {
    return RHIVertexBindingDescription{.binding = 0,
                                       .stride = sizeof(Vertex),
                                       .inputRate = RHIVertexInputRate::Vertex};
  }

  static std::array<RHIVertexAttributeDescription, 3>
  getAttributeDescription() {
    std::array<RHIVertexAttributeDescription, 3> descriptions;

    descriptions[0] = RHIVertexAttributeDescription{
        .location = 0,
        .binding = 0,
        .format = RHIFormat::R32G32Sfloat,
        .offset = offsetof(Vertex, position),
    };

    descriptions[1] = RHIVertexAttributeDescription{
        .location = 1,
        .binding = 0,
        .format = RHIFormat::R32G32B32Sfloat,
        .offset = offsetof(Vertex, color),
    };

    descriptions[2] = RHIVertexAttributeDescription{
        .location = 2,
        .binding = 0,
        .format = RHIFormat::R32G32Sfloat,
        .offset = offsetof(Vertex, texCoord),
    };

    return descriptions;
  }
};

struct Transform {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 projection;
};

}  // namespace Sparrow

#endif