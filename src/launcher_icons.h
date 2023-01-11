#pragma once

#include <zukou.h>

#include <vector>

#include "config.h"
#include "cuboid.h"
#include "icon-background.h"
#include "icon.h"

#define ICON_REGION_HALF_SIZE 0.025
#define ICON_REGION_THICKNESS_SIZE 0.00005

namespace zennist {

class LauncherIcons
{
 public:
  DISABLE_MOVE_AND_COPY(LauncherIcons);
  LauncherIcons() = delete;
  LauncherIcons(zukou::System* system, zukou::Expansive* expansive);

  bool Render(Config* config);

  void RayLeave();
  void RayMotion(glm::vec3 origin, glm::vec3 direction);
  void RayButton(uint32_t button, bool pressed);

 private:
  zukou::System* system_;
  zukou::Expansive* expansive_;
  std::vector<Cuboid> cuboid_list_ = {};
  std::vector<Icon*> icon_list_ = {};
  std::vector<IconBackground*> icon_background_list_ = {};
  std::vector<FavoriteApp> favorite_app_list_ = {};

  int focus_index_ = -1;

  void Launch(FavoriteApp* app);
  float IntersectRayObb(glm::vec3 origin, glm::vec3 direction,
      glm::vec3 aabb_half_size, glm::mat4 transform);
  int IntersectAxisTest(glm::vec3 axis, glm::vec3 translation,
      glm::vec3 ray_direction, float axis_min, float axis_max, float* t_min,
      float* t_max);
};

}  // namespace zennist
