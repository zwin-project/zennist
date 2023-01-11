#include "launcher_icons.h"

#include <glm/gtx/quaternion.hpp>
#include <iostream>

namespace zennist {

LauncherIcons::LauncherIcons(zukou::System* system, zukou::Expansive* expansive)
    : system_(system), expansive_(expansive)
{}

bool
LauncherIcons::Init(std::vector<int> icon_list)
{
  zukou::Region region(system_);
  if (!region.Init()) {
    std::cerr << "Failed to initialize region." << std::endl;
    return false;
  }

  float radius = 0.63;
  float desk_height = 0.64;
  float default_theta = M_PI / 2.0;
  float surface_theta = -M_PI / 4.0;

  float unit_theta = M_PI / 80.0;
  glm::vec3 adjust_diff(-0.006);

  int start = icon_list.size() - 1;
  for (int i = start; i >= -start; i -= 2) {
    float theta = i * unit_theta + default_theta;

    glm::mat4 R = glm::rotate(glm::mat4(1), theta, glm::vec3(0, 1, 0)) *
                  glm::rotate(glm::mat4(1), surface_theta, glm::vec3(0, 0, 1));
    glm::quat quaternion = glm::toQuat(R);

    float x = radius * cos(theta);
    float z = -radius * sin(theta);
    glm::vec3 center = {x, desk_height, z};

    Cuboid cuboid(
        glm::vec3(0.00005, ICON_REGION_HALF_SIZE, ICON_REGION_HALF_SIZE),
        center, quaternion);

    cuboid_list_.push_back(cuboid);

    region.AddCuboid(
        cuboid.half_size + adjust_diff, cuboid.center, cuboid.quaternion);

    Icon* icon = new Icon(system_, expansive_);
    icon->Render(cuboid);
    icon_list_.push_back(icon);
  }

  expansive_->SetRegion(&region);

  return true;
}

bool
LauncherIcons::Render(float radius, glm::mat4 transform)
{
  (void)radius;
  (void)transform;

  return true;
}

void
LauncherIcons::RayMotion(glm::vec3 origin, glm::vec3 direction)
{
  float min_distance = FLT_MAX;
  int i = 0;
  int min_i = -1;
  for (auto cuboid : cuboid_list_) {
    glm::mat4 transform = glm::translate(glm::mat4(1), cuboid.center) *
                          glm::toMat4(cuboid.quaternion);
    float distance =
        IntersectRayObb(origin, direction, cuboid.half_size, transform);
    if (distance < min_distance) {
      min_distance = distance;
      min_i = i;
    }
    i++;
  }

  cuboid_index_ = min_i;
}

void
LauncherIcons::RayButton(uint32_t button, bool pressed)
{
  (void)button;
  (void)pressed;
  if (cuboid_index_ != -1 && !pressed) std::cout << cuboid_index_ << std::endl;
}

float
LauncherIcons::IntersectRayObb(glm::vec3 origin, glm::vec3 direction,
    glm::vec3 aabb_half_size, glm::mat4 transform)
{
  float t_min = 0.0f;
  float t_max = FLT_MAX;

  glm::vec3 obb_position_worldspace = transform[3];
  glm::vec3 translation = obb_position_worldspace - origin;

  for (int i = 0; i < 3; i++) {  // test x, y and z axis
    glm::vec3 axis = transform[i];
    if (IntersectAxisTest(axis, translation, direction, -aabb_half_size[i],
            aabb_half_size[i], &t_min, &t_max) == -1)
      return FLT_MAX;
  }

  return t_min;
}

int
LauncherIcons::IntersectAxisTest(glm::vec3 axis, glm::vec3 translation,
    glm::vec3 ray_direction, float axis_min, float axis_max, float* t_min,
    float* t_max)
{
  float e = glm::dot(axis, translation);
  float f = glm::dot(ray_direction, axis);

  if (fabs(f) > 0.001f) {
    float t1 = (e + axis_min) / f;
    float t2 = (e + axis_max) / f;
    if (t1 > t2) std::swap(t1, t2);
    if (t2 < *t_max) *t_max = t2;
    if (*t_min < t1) *t_min = t1;
    if (*t_max < *t_min) return -1;
  } else {
    if (-e + axis_min > 0.0f || -e + axis_max < 0.0f) return -1;
  }
  return 0;
}

}  // namespace zennist
