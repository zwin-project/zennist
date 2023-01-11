#pragma once

#include <zukou.h>

#include <vector>

#include "cuboid.h"
#include "icon.h"

#define ICON_REGION_HALF_SIZE 0.025

namespace zennist {

class LauncherIcons
{
 public:
  DISABLE_MOVE_AND_COPY(LauncherIcons);
  LauncherIcons() = delete;
  LauncherIcons(zukou::System* system, zukou::Expansive* expansive);

  bool Init(std::vector<int> icon_list);

  bool Render(float radius, glm::mat4 transform);

  void RayMotion(glm::vec3 origin, glm::vec3 direction);
  void RayButton(uint32_t button, bool pressed);

 private:
  zukou::System* system_;
  zukou::Expansive* expansive_;
  std::vector<Cuboid> cuboid_list_;
  std::vector<Icon*> icon_list_;

  int cuboid_index_ = -1;

  float IntersectRayObb(glm::vec3 origin, glm::vec3 direction,
      glm::vec3 aabb_half_size, glm::mat4 transform);
  int IntersectAxisTest(glm::vec3 axis, glm::vec3 translation,
      glm::vec3 ray_direction, float axis_min, float axis_max, float* t_min,
      float* t_max);
};

}  // namespace zennist
