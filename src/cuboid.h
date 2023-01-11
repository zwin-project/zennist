#pragma once

#include <zukou.h>

namespace zennist {

struct Cuboid {
  Cuboid(glm::vec3 half_size, glm::vec3 center, glm::quat quaternion)
      : half_size(half_size), center(center), quaternion(quaternion)
  {}
  glm::vec3 half_size;
  glm::vec3 center;
  glm::quat quaternion;
};

}  // namespace zennist
